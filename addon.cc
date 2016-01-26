#include "jp2a-1.0.6/include/jp2a.h"

#include <fstream>
#include <node.h>
#include <stdio.h>

using v8::FunctionCallbackInfo;
using v8::Handle;
using v8::Isolate;
using v8::Object;
using v8::String;
using v8::Value;

void Decode(const FunctionCallbackInfo<Value> &arguments) {
  Isolate *isolate = arguments.GetIsolate();
  if (arguments.Length() < 1)
    return;
  String::Utf8Value filename(arguments[0]->ToString());
  std::ifstream is(*filename, std::ifstream::binary);
  if (is) {
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
  }
  // arguments.GetReturnValue().Set();
}

void Init(Handle<Object> exports) {
  NODE_SET_METHOD(exports, "decode", Decode);
}

NODE_MODULE(jp2a, Init)
