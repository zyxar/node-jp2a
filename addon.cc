#include "jp2a-1.0.6/include/jp2a.h"
#include "jp2a-1.0.6/config.h"

#include <fstream>
#include <node.h>
#include <stdio.h>

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
    decompress_mem(buffer, length, stderr);
  }
  is.close();
  if (buffer)
    delete[] buffer;
  // arguments.GetReturnValue().Set();
}

void Init(Handle<Object> exports, Handle<Object> module) {
  Isolate *isolate = exports->GetIsolate();
  Local<Function> fn = FunctionTemplate::New(isolate, Jp2a)->GetFunction();
  fn->SetName(String::NewFromUtf8(isolate, "jp2a"));
  Local<Object> versions = Object::New(isolate);
  Local<String> version = String::NewFromUtf8(isolate, "0.1.0");
  versions->Set(String::NewFromUtf8(isolate, "package"), version);
  versions->Set(String::NewFromUtf8(isolate, "jp2a"),
                String::NewFromUtf8(isolate, VERSION));
  fn->Set(String::NewFromUtf8(isolate, "versions"), versions);
  fn->Set(String::NewFromUtf8(isolate, "version"), version);
  module->Set(String::NewFromUtf8(isolate, "exports"), fn);
}

NODE_MODULE(jp2a, Init)
