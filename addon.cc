#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include <node.h>
#include <fstream>
#include <string>
#include <stdio.h>

#include "jp2a.h"

using namespace v8;

Handle<Value> Do(const Arguments& args) {
  v8::String::Utf8Value filename(args[0]->ToString());
  std::ifstream is(std::string(*filename).c_str(), std::ifstream::binary);
  char* buffer = NULL;
  int length = 0;
  if (is) {
    is.seekg(0, is.end);
    length = is.tellg();
    is.seekg(0, is.beg);
    buffer = new char[length];
    is.read(buffer, length);
    if (is) {
      init_options();
      decompress_mem(buffer, length, stderr);
    }
    is.close();
    delete[] buffer;
  }
  HandleScope scope;
  return scope.Close(Undefined());
}

Handle<Value> CreateFunction(const Arguments& args) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(Do);
  Local<Function> fn = tpl->GetFunction();
  fn->SetName(String::NewSymbol("Do"));

  return scope.Close(fn);
}

void Init(Handle<Object> exports) {
  exports->Set(String::NewSymbol("decoder"),
      FunctionTemplate::New(CreateFunction)->GetFunction());
}

NODE_MODULE(jp2a, Init)
