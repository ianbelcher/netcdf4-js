#include "Attribute.h"
#include <inttypes.h>
#include <netcdf.h>
#include <iostream>
#include "netcdf4js.h"
#include <nan.h>

namespace netcdf4js {

v8::Persistent<v8::Function> Attribute::constructor;

Attribute::Attribute(const char* name_, int var_id_, int parent_id_) : name(name_), var_id(var_id_), parent_id(parent_id_) {
    call_netcdf(nc_inq_atttype(parent_id, var_id_, name_, &type));
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {};
    v8::Local<v8::Function> cons = v8::Local<v8::Function>::New(isolate, constructor);
    Wrap(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
}

Attribute::Attribute(const char* name_, int var_id_, int parent_id_, int type_) : name(name_), var_id(var_id_), parent_id(parent_id_), type(type_) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {};
    v8::Local<v8::Function> cons = v8::Local<v8::Function>::New(isolate, constructor);
    Wrap(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
}

void Attribute::Init(v8::Local<v8::Object> exports) {
    v8::Isolate* isolate = exports->GetIsolate();
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate);
    tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Attribute"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    NODE_SET_PROTOTYPE_METHOD(tpl, "delete", Attribute::Delete);
    NODE_SET_PROTOTYPE_METHOD(tpl, "inspect", Attribute::Inspect);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "name"), Attribute::GetName, Attribute::SetName);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "value"), Attribute::GetValue, Attribute::SetValue);
    constructor.Reset(isolate, tpl->GetFunction());
}

void Attribute::GetName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Attribute* obj = node::ObjectWrap::Unwrap<Attribute>(info.Holder());
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, obj->name.c_str()));
}

void Attribute::SetName(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info) {
    Attribute* obj = node::ObjectWrap::Unwrap<Attribute>(info.Holder());
    v8::String::Utf8Value new_name_(val->ToString());
    call_netcdf(nc_rename_att(obj->parent_id, obj->var_id, obj->name.c_str(), *new_name_));
    obj->name = *new_name_;
}

void Attribute::GetValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Attribute* obj = node::ObjectWrap::Unwrap<Attribute>(info.Holder());

    if ((obj->type < NC_BYTE || obj->type > NC_UINT64) && obj->type != NC_STRING) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet")));
        return;
    }

    size_t len;
    call_netcdf(nc_inq_attlen(obj->parent_id, obj->var_id, obj->name.c_str(), &len));

    switch (obj->type) {
        case NC_BYTE: {
            if (len == 1) {
                int8_t v;
                call_netcdf(nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), &v));
                info.GetReturnValue().Set(v8::Integer::New(isolate, v));
            } else {
                std::vector<int8_t> v(len);
                call_netcdf(nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), &v[0]));
                info.GetReturnValue().Set(v8::Int8Array::New(v8::ArrayBuffer::New(isolate, &v[0], len * 1), 0, len));
            }
        } break;
        case NC_SHORT: {
            if (len == 1) {
                int16_t v;
                call_netcdf(nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), &v));
                info.GetReturnValue().Set(v8::Integer::New(isolate, v));
            } else {
                std::vector<int16_t> v(len);
                call_netcdf(nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), &v));
                info.GetReturnValue().Set(v8::Int16Array::New(v8::ArrayBuffer::New(isolate, &v[0], len * 2), 0, len));
            }
        } break;
        case NC_INT: {
            if (len == 1) {
                int32_t v;
                call_netcdf(nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), &v));
                info.GetReturnValue().Set(v8::Integer::New(isolate, v));
            } else {
                std::vector<int32_t> v(len);
                call_netcdf(nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), &v));
                info.GetReturnValue().Set(v8::Int32Array::New(v8::ArrayBuffer::New(isolate, &v[0], len * 4), 0, len));
            }
        } break;
        case NC_FLOAT: {
            if (len == 1) {
                float v;
                call_netcdf(nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), &v));
                info.GetReturnValue().Set(v8::Number::New(isolate, v));
            } else {
                std::vector<float> v(len);
                call_netcdf(nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), &v));
                info.GetReturnValue().Set(v8::Float32Array::New(v8::ArrayBuffer::New(isolate, &v[0], len * 4), 0, len));
            }
        } break;
        case NC_DOUBLE: {
            if (len == 1) {
                double v;
                call_netcdf(nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), &v));
                info.GetReturnValue().Set(v8::Number::New(isolate, v));
            } else {
                std::vector<double> v(len);
                call_netcdf(nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), &v));
                info.GetReturnValue().Set(v8::Float64Array::New(v8::ArrayBuffer::New(isolate, &v[0], len * 8), 0, len));
            }
        } break;
        case NC_UBYTE: {
            if (len == 1) {
                uint8_t v;
                call_netcdf(nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), &v));
                info.GetReturnValue().Set(v8::Integer::New(isolate, v));
            } else {
                std::vector<uint8_t> v(len);
                call_netcdf(nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), &v));
                info.GetReturnValue().Set(v8::Uint8Array::New(v8::ArrayBuffer::New(isolate, &v[0], len * 1), 0, len));
            }
        } break;
        case NC_USHORT: {
            if (len == 1) {
                uint16_t v;
                call_netcdf(nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), &v));
                info.GetReturnValue().Set(v8::Integer::New(isolate, v));
            } else {
                std::vector<uint16_t> v(len);
                call_netcdf(nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), &v));
                info.GetReturnValue().Set(v8::Uint16Array::New(v8::ArrayBuffer::New(isolate, &v[0], len * 2), 0, len));
            }
        } break;
        case NC_UINT: {
            if (len == 1) {
                uint32_t v;
                call_netcdf(nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), &v));
                info.GetReturnValue().Set(v8::Integer::New(isolate, v));
            } else {
                std::vector<uint32_t> v(len);
                call_netcdf(nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), &v));
                info.GetReturnValue().Set(v8::Uint32Array::New(v8::ArrayBuffer::New(isolate, &v[0], len * 4), 0, len));
            }
        } break;
        case NC_INT64: {
            if (len == 1) {
                int64_t v;
                call_netcdf(nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), &v));
                info.GetReturnValue().Set(v8::Integer::New(isolate, v));
            } else {
                std::vector<int64_t> v(len);
                call_netcdf(nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), &v));
                info.GetReturnValue().Set(v8::Int32Array::New(v8::ArrayBuffer::New(isolate, &v[0], len * 8), 0, len));
            }
        } break;
        case NC_UINT64: {
            if (len == 1) {
                uint64_t v;
                call_netcdf(nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), &v));
                info.GetReturnValue().Set(v8::Integer::New(isolate, v));
            } else {
                std::vector<uint64_t> v(len);
                call_netcdf(nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), &v));
                info.GetReturnValue().Set(v8::Uint32Array::New(v8::ArrayBuffer::New(isolate, &v[0], len * 8), 0, len));
            }
        } break;
        case NC_CHAR:
        case NC_STRING: {
            std::vector<char> v(len + 1);
            v[len] = 0;
            call_netcdf(nc_get_att_text(obj->parent_id, obj->var_id, obj->name.c_str(), &v[0]));
            info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, &v[0]));
        } break;
    }
}

void Attribute::SetValue(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info) {
    Attribute* obj = node::ObjectWrap::Unwrap<Attribute>(info.Holder());
    obj->set_value(val);
}

void Attribute::set_value(const v8::Local<v8::Value>& val) {
    if ((type < NC_BYTE || type > NC_UINT) && type != NC_STRING) {
        v8::Isolate::GetCurrent()->ThrowException(
            v8::Exception::TypeError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "Variable type not supported yet")));
        return;
    }

    if (val->IsUint32()) {
        uint32_t v = val->Uint32Value();
        call_netcdf(nc_put_att(parent_id, var_id, name.c_str(), NC_UINT, 1, &v));
    } else if (val->IsInt32()) {
        int32_t v = val->Int32Value();
        call_netcdf(nc_put_att(parent_id, var_id, name.c_str(), NC_INT, 1, &v));
    } else if (val->IsNumber()) {
        double v = val->NumberValue();
        call_netcdf(nc_put_att(parent_id, var_id, name.c_str(), NC_DOUBLE, 1, &v));
    } else {
        std::string v(*v8::String::Utf8Value(val->ToString()));
        call_netcdf(nc_put_att_text(parent_id, var_id, name.c_str(), v.length(), v.c_str()));
    }
}

void Attribute::Delete(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Attribute* obj = node::ObjectWrap::Unwrap<Attribute>(args.Holder());
    call_netcdf(nc_del_att(obj->parent_id, obj->var_id, obj->name.c_str()));
}

void Attribute::Inspect(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "[object Attribute]"));
}
}  // namespace netcdf4js
