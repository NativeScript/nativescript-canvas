#include "V8GlobalHelpers.h"

using namespace v8;
using namespace std;

string ConvertToString(const v8::Local<String>& s)
{
	if (s.IsEmpty())
	{
		return string();
	}
	else
	{
		String::Utf8Value str(s);
		return string(*str);
	}
}

Local<String> ConvertToV8String(const jchar* data, int length)
{
	auto isolate = Isolate::GetCurrent();
	return String::NewFromTwoByte(isolate, (const uint16_t*)data, String::kNormalString, length);
}

Local<String> ConvertToV8String(const string& s)
{
	auto isolate = Isolate::GetCurrent();
	return String::NewFromUtf8(isolate, s.c_str());
}

Local<String> ConvertToV8String(const char *data, int length)
{
	auto isolate = Isolate::GetCurrent();
	return String::NewFromUtf8(isolate, (const char *)data, String::kNormalString, length);
}

Local<Value> V8GetHiddenValue(const Local<Object>& obj, const string& propName)
{
	auto s = ConvertToV8String(propName);
	return obj->GetHiddenValue(s);
}

bool V8SetHiddenValue(const Local<Object>& obj, const string& propName, const Local<Value>& value)
{
	auto s = ConvertToV8String(propName);
	return obj->SetHiddenValue(s, value);
}
