#include "jp2a-1.0.6/config.h"
#include "jp2a-1.0.6/include/jp2a.h"
#include "jp2a-1.0.6/include/options.h"
#include "Image.h"

#include <fstream>
#include <node.h>

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
using JP2A::Image;

void processJPEG(Isolate *isolate, const char *filename,
                 std::stringstream &ss) {
  std::ifstream is(filename, std::ifstream::binary);
  if (!is) {
    isolate->ThrowException(
        Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments")));
    return;
  }
  is.seekg(0, is.end);
  int length = is.tellg();
  is.seekg(0, is.beg);
  char *buffer = new char[length];
  is.read(buffer, length);
  is.close();
  Image image(reinterpret_cast<unsigned char *>(buffer), length);
  delete[] buffer;
  std::string errorMessage = image.errorMessage();
  if (errorMessage.length() > 0) {
    isolate->ThrowException(
        Exception::Error(String::NewFromUtf8(isolate, errorMessage.c_str())));
    return;
  }
  image >> ss;
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
  init_options(); // TODO: handle options

  String::Utf8Value filename(arguments[0]->ToString());
  std::stringstream ss;
  processJPEG(isolate, *filename, ss);
  arguments.GetReturnValue().Set(
      String::NewFromUtf8(isolate, ss.str().c_str()));
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
