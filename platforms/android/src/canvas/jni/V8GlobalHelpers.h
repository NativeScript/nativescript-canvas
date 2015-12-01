#ifndef V8GLOBALHELPERS_H_
#define V8GLOBALHELPERS_H_

#include "jni.h"
#include "v8.h"
#include <string>

std::string ConvertToString(const v8::Local<v8::String>& s);

jstring ConvertToJavaString(const v8::Local<v8::Value>& jsValue);

template <size_t N>
v8::Local<v8::String> ConvertToV8String(jchar (&data)[N])
{
	auto isolate = v8::Isolate::GetCurrent();
	return v8::String::NewFromUtf8(isolate, (const char *)data, v8::String::kNormalString, N);
}

v8::Local<v8::String> ConvertToV8String(const jchar* data, int length);

v8::Local<v8::String> ConvertToV8String(const std::string& s);

v8::Local<v8::String> ConvertToV8String(const char *data, int length);

v8::Local<v8::Value> V8GetHiddenValue(const v8::Local<v8::Object>& obj, const std::string& propName);

bool V8SetHiddenValue(const v8::Local<v8::Object>& obj, const std::string& propName, const v8::Local<v8::Value>& value);

#endif /* V8GLOBALHELPERS_H_ */
