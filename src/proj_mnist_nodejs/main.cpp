#include <iostream>
#include <node/node.h>
#include "ann/mlp.h"
#include <vector>
#include <memory>

static std::shared_ptr<ann::MLP> network;

void recognizeImage ( const v8::FunctionCallbackInfo<v8::Value> & args ) {
    v8::Isolate * isolate = args.GetIsolate();

    if ( args.Length() < 1 ) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return ;
    }

    if ( !args[0]->IsArray() ) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Argument must be array")));
        return;
    }
    
    const size_t sizeInputs = 28 * 28;
    v8::Local<v8::Array> jsArray = v8::Local<v8::Array>::Cast(args[0]);

    if ( jsArray->Length() != sizeInputs ) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Invalid size array")));
        return;
    }

    std::vector<double> image;

    for ( size_t index = 0; index < jsArray->Length(); index++ ) {
        v8::Handle<v8::Value> item = jsArray->Get(index);
        image.push_back(item->NumberValue());
    }

    std::vector<double> outputs = network->train(image);

    v8::Local<v8::Array> jsResult = v8::Array::New(isolate, outputs.size());

    for ( size_t index = 0; index < outputs.size(); index++ ) {        
        jsResult->Set(index, v8::Number::New(isolate, outputs[index]));
    }

    args.GetReturnValue().Set(jsResult);
}

void initMlp ( const v8::FunctionCallbackInfo<v8::Value> & args ) {
    v8::Isolate * isolate = args.GetIsolate();

    if ( args.Length() < 1 ) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return ;
    }

    v8::String::Utf8Value param(args[0]->ToString());
    std::string backPath(*param);
    boost::optional<std::shared_ptr<ann::MLP>> mlp_ = ann::MLP::load(backPath);

    if ( !mlp_ ) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Couldn't open file")));
        return ;
    }

    network = *mlp_;
}


void init ( v8::Local<v8::Object> exports )
{
    NODE_SET_METHOD(exports, "init", initMlp);
    NODE_SET_METHOD(exports, "recognize", recognizeImage);
}

NODE_MODULE(mlp, init)