#include "Dimension.h"
#include <netcdf.h>
#include "netcdf4js.h"
#include <nan.h>

namespace netcdf4js {

v8::Persistent<v8::Function> Dimension::constructor;

Dimension::Dimension(const int& id_, const int& parent_id_) : id(id_), parent_id(parent_id_) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {};
    v8::Local<v8::Function> cons = v8::Local<v8::Function>::New(isolate, constructor);
    Wrap(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
}

void Dimension::Init(v8::Local<v8::Object> exports) {
    v8::Isolate* isolate = exports->GetIsolate();
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate);
    tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Dimension"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    NODE_SET_PROTOTYPE_METHOD(tpl, "inspect", Dimension::Inspect);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "id"), Dimension::GetId);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "length"), Dimension::GetLength);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "name"), Dimension::GetName, Dimension::SetName);
    constructor.Reset(isolate, tpl->GetFunction());
}

bool Dimension::get_name(char* name) const {
    call_netcdf_bool(nc_inq_dimname(parent_id, id, name));
    return true;
}

void Dimension::GetId(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Dimension* obj = node::ObjectWrap::Unwrap<Dimension>(info.Holder());
    info.GetReturnValue().Set(v8::Integer::New(isolate, obj->id));
}

void Dimension::GetLength(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Dimension* obj = node::ObjectWrap::Unwrap<Dimension>(info.Holder());
    size_t len;
    call_netcdf(nc_inq_dimlen(obj->parent_id, obj->id, &len));
    info.GetReturnValue().Set(v8::Integer::New(isolate, len));
}

void Dimension::GetName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Dimension* obj = node::ObjectWrap::Unwrap<Dimension>(info.Holder());
    char name[NC_MAX_NAME + 1];
    if (obj->get_name(name)) {
        info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, name));
    }
}

void Dimension::SetName(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info) {
    Dimension* obj = node::ObjectWrap::Unwrap<Dimension>(info.Holder());
    v8::String::Utf8Value new_name_(val->ToString());
    call_netcdf(nc_rename_dim(obj->parent_id, obj->id, *new_name_));
}

void Dimension::Inspect(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "[object Dimension]"));
}
}  // namespace netcdf4js
