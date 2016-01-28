#include "jp2a-1.0.6/config.h"
#include "jp2a-1.0.6/include/jp2a.h"
#include "jp2a-1.0.6/include/options.h"

#include <fstream>
#include <jpeglib.h>
#include <node.h>
#include <stdio.h>
#include <sstream>

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Function;
using v8::Handle;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

extern "C" {
typedef struct Image_ {
  int width;
  int height;
  float *pixel;
  float *red, *green, *blue;
  int *yadds;
  float resize_y;
  float resize_x;
  int *lookup_resx;
} Image;

extern void aspect_ratio(const int, const int);
extern void print_image(const Image *const, const int, FILE *);
extern void print_image_colors(const Image *const, const int, FILE *);
extern void malloc_image(Image *);
extern void clear(Image *);
extern void init_image(Image *, const struct jpeg_decompress_struct *);
extern void process_scanline(const struct jpeg_decompress_struct *,
                             const JSAMPLE *, Image *);
extern void print_border(const int);
extern void free_image(Image *);
extern void normalize(Image *);
}

void processImage(Isolate *isolate, void *buffer, long length, FILE *fout) {
  struct jpeg_error_mgr jerr;
  struct jpeg_decompress_struct jpg;
  jpg.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&jpg);
  jpeg_mem_src(&jpg, reinterpret_cast<unsigned char *>(buffer), length);
  jpeg_read_header(&jpg, TRUE);
  jpeg_start_decompress(&jpg);

  if (jpg.data_precision != 8) {
    std::stringstream ss;
    ss << "Image has " << jpg.data_precision
       << " bits color channels, we only support 8-bit.";
    isolate->ThrowException(
        Exception::Error(String::NewFromUtf8(isolate, ss.str().c_str())));
    return;
  }

  int row_stride = jpg.output_width * jpg.output_components;

  JSAMPARRAY jbuffer =
      (*jpg.mem->alloc_sarray)((j_common_ptr)&jpg, JPOOL_IMAGE, row_stride, 1);

  aspect_ratio(jpg.output_width, jpg.output_height);

  Image image;
  malloc_image(&image);
  clear(&image);

  // if (verbose)
  //   print_info(&jpg);

  init_image(&image, &jpg);

  while (jpg.output_scanline < jpg.output_height) {
    jpeg_read_scanlines(&jpg, jbuffer, 1);
    process_scanline(&jpg, jbuffer[0], &image);
  }

  normalize(&image);

  if (use_border)
    print_border(image.width);

  (!usecolors ? print_image : print_image_colors)(
      &image, (int)strlen(ascii_palette) - 1, fout);

  if (use_border)
    print_border(image.width);

  free_image(&image);
  jpeg_finish_decompress(&jpg);
  jpeg_destroy_decompress(&jpg);
}

void Jp2a(const FunctionCallbackInfo<Value> &arguments) {
  Isolate *isolate = arguments.GetIsolate();
  if (arguments.IsConstructCall()) {
    Local<Function> fn = FunctionTemplate::New(isolate, Jp2a)->GetFunction();
    fn->SetName(String::NewFromUtf8(isolate, "jp2a"));
    return arguments.GetReturnValue().Set(fn);
  }
  if (arguments.Length() < 1 || !arguments[0]->IsString()) {
    isolate->ThrowException(
        Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments")));
    return;
  }
  String::Utf8Value filename(arguments[0]->ToString());
  std::ifstream is(*filename, std::ifstream::binary);
  if (!is) {
    isolate->ThrowException(
        Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments")));
    return;
  }
  // TODO: handle options
  is.seekg(0, is.end);
  int length = is.tellg();
  is.seekg(0, is.beg);
  char *buffer = new char[length];
  is.read(buffer, length);
  if (is) {
    init_options();
    processImage(isolate, buffer, length, stderr);
  }
  is.close();
  if (buffer)
    delete[] buffer;
  // arguments.GetReturnValue().Set();
}

void Init(Handle<Object> exports, Handle<Object> module) {
  Isolate *isolate = exports->GetIsolate();
  Local<Function> fn = FunctionTemplate::New(isolate, Jp2a)->GetFunction();
  Local<String> jp2a = String::NewFromUtf8(isolate, "jp2a");
  fn->SetName(jp2a);
  fn->ForceSet(String::NewFromUtf8(isolate, "version"),
               String::NewFromUtf8(isolate, VERSION), v8::ReadOnly);
  module->Set(String::NewFromUtf8(isolate, "exports"), fn);
}

NODE_MODULE(jp2a, Init)
