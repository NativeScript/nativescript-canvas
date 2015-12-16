#include "CanvasRenderingContext2D.h"
#include "CanvasRenderingContextGL.h"

using namespace v8;

void ExportFactory(Isolate* isolate, Local<Object>& exports, const char* factoryName, FunctionCallback impl)
{
	auto templ = v8::FunctionTemplate::New(isolate, impl);
	auto ctx = isolate->GetCurrentContext();
	exports->Set(ctx,
			v8::String::NewFromUtf8(isolate, factoryName),
			templ->GetFunction(ctx).ToLocalChecked());
}

extern "C" void NSMain(const FunctionCallbackInfo<Value>& args)
{
	auto isolate = args.GetIsolate();
	auto exports = args[1].As<Object>();

	ExportFactory(isolate, exports, "create2d", &CanvasRenderingContext2D::Create);
	ExportFactory(isolate, exports, "createGL", &CanvasRenderingContextGL::Create);
}
