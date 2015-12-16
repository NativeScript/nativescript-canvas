/*
 * CanvasRenderingContext2D.cpp
 *
 *  Created on: Nov 12, 2015
 *      Author: dragnev
 */

#include "CanvasRenderingContext2D.h"
#include "V8GlobalHelpers.h"

#include "SkSurface.h"
#include "SkCanvas.h"
#include "SkGraphics.h"
#include "SkPath.h"

#include <android/bitmap.h>
#include <android/log.h>

#include <algorithm>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

using namespace v8;

static double DegFromRad(double radAngle)
{
	return radAngle * 180.0 / M_PI;
}

template <class TObject>
static CanvasRenderingContext2D* ToContext(const TObject& args)
{
	auto self = args.Holder();
	auto wrap = Local<External>::Cast(self->GetInternalField(0));
	return static_cast<CanvasRenderingContext2D*>(wrap->Value());
}

CanvasRenderingContext2D::CanvasRenderingContext2D()
{
	myWidth = 0;
	myHeight = 0;

	myStrokeColor = SK_ColorBLACK;
	myStrokeStyle = "#000";

	myFillColor = SK_ColorBLACK;
	myFillStyle = "#000";

	mySurface = NULL;
	myCanvas = NULL;

	myPaint = new SkPaint();
	myPaint->setAntiAlias(true);

	myPath = new SkPath();

	myClearPaint = new SkPaint();
	myClearPaint->setXfermodeMode(SkXfermode::kClear_Mode);

	CreateBackbuffer(100, 100);
}

CanvasRenderingContext2D::~CanvasRenderingContext2D()
{
	if (mySurface)
		mySurface->unref();
	delete myPaint;
	delete myPath;
	delete myClearPaint;
}

void CanvasRenderingContext2D::Create(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	auto isolate = args.GetIsolate();
	HandleScope scope(isolate);

	Local<ObjectTemplate> contextTemplate = ObjectTemplate::New(isolate);
	contextTemplate->SetInternalFieldCount(1);

	contextTemplate->Set(ConvertToV8String("__draw"), FunctionTemplate::New(isolate, &Draw));
	contextTemplate->Set(ConvertToV8String("__sizeChanged"), FunctionTemplate::New(isolate, &SizeChanged));

	contextTemplate->SetAccessor(ConvertToV8String("fillStyle"), &GetFillStyle, &SetFillStyle);
	contextTemplate->SetAccessor(ConvertToV8String("strokeStyle"), &GetStrokeStyle, &SetStrokeStyle);

	contextTemplate->Set(ConvertToV8String("arc"), FunctionTemplate::New(isolate, &Arc));
	contextTemplate->Set(ConvertToV8String("beginPath"), FunctionTemplate::New(isolate, &BeginPath));
	contextTemplate->Set(ConvertToV8String("bezierCurveTo"), FunctionTemplate::New(isolate, &BezierCurveTo));
	contextTemplate->Set(ConvertToV8String("clearRect"), FunctionTemplate::New(isolate, &ClearRect));
	contextTemplate->Set(ConvertToV8String("closePath"), FunctionTemplate::New(isolate, &ClosePath));
	contextTemplate->Set(ConvertToV8String("drawImage"), FunctionTemplate::New(isolate, &DrawImage));
	contextTemplate->Set(ConvertToV8String("fill"), FunctionTemplate::New(isolate, &Fill));
	contextTemplate->Set(ConvertToV8String("fillRect"), FunctionTemplate::New(isolate, &FillRect));
	contextTemplate->Set(ConvertToV8String("fillText"), FunctionTemplate::New(isolate, &FillText));
	contextTemplate->Set(ConvertToV8String("getImageData"), FunctionTemplate::New(isolate, &GetImageData));
	contextTemplate->Set(ConvertToV8String("lineTo"), FunctionTemplate::New(isolate, &LineTo));
	contextTemplate->Set(ConvertToV8String("measureText"), FunctionTemplate::New(isolate, &MeasureText));
	contextTemplate->Set(ConvertToV8String("moveTo"), FunctionTemplate::New(isolate, &MoveTo));
	contextTemplate->Set(ConvertToV8String("quadraticCurveTo"), FunctionTemplate::New(isolate, &QuadraticCurveTo));
	contextTemplate->Set(ConvertToV8String("restore"), FunctionTemplate::New(isolate, &Restore));
	contextTemplate->Set(ConvertToV8String("rotate"), FunctionTemplate::New(isolate, &Rotate));
	contextTemplate->Set(ConvertToV8String("save"), FunctionTemplate::New(isolate, &Save));
	contextTemplate->Set(ConvertToV8String("stroke"), FunctionTemplate::New(isolate, &Stroke));
	contextTemplate->Set(ConvertToV8String("translate"), FunctionTemplate::New(isolate, &Translate));

	Local<Object> newContext = contextTemplate->NewInstance();
	newContext->ForceSet(ConvertToV8String("canvas"), args[0]);
	newContext->ForceSet(ConvertToV8String("__kind"), ConvertToV8String("2d"));

	auto nativeContext = new CanvasRenderingContext2D();
	newContext->SetInternalField(0, External::New(isolate, nativeContext));

	Persistent<Object> persistentHandle(isolate, newContext);
	persistentHandle.SetWeak(nativeContext, &Deallocate);

	args.GetReturnValue().Set(newContext);
}

void CanvasRenderingContext2D::Deallocate(const v8::WeakCallbackData<v8::Object, CanvasRenderingContext2D>& data)
{
	__android_log_print(ANDROID_LOG_DEBUG, "C2D", "DEALLOCATING!");
	delete data.GetParameter();
}

void CanvasRenderingContext2D::DeallocateBuffer(const v8::WeakCallbackData<v8::ArrayBuffer, char>& data)
{
	delete[] data.GetParameter();
}

void CanvasRenderingContext2D::CreateBackbuffer(int width, int height)
{
	if (mySurface)
	{
		mySurface->unref();
		mySurface = NULL;
	}

	myWidth = width;
	myHeight = height;
	auto info = SkImageInfo::MakeN32Premul(width, height);
	size_t rowBytes = info.minRowBytes();
	size_t size = info.getSafeSize(rowBytes);
	myPixelMemory.resize(size);
	mySurface = SkSurface::NewRasterDirect(info, &myPixelMemory[0], rowBytes);
	myCanvas = mySurface->getCanvas();
}

void CanvasRenderingContext2D::Draw(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	auto context = ToContext(args);

	auto pixelsPtrLow = args[0]->Int32Value();
	auto pixelsPtrHigh = args[1]->Int32Value();
	auto pixelsPtrLong = pixelsPtrLow | (int64_t(pixelsPtrHigh) << 32);
	void* pixelsPtr = (void*) pixelsPtrLong;

	auto width = args[2]->Int32Value();
	auto height = args[3]->Int32Value();

	memcpy(pixelsPtr, &context->myPixelMemory[0], width * height * 4);
}

void CanvasRenderingContext2D::SizeChanged(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	auto context = ToContext(args);
	auto width = args[0]->Int32Value();
	auto height = args[1]->Int32Value();
	context->CreateBackbuffer(width, height);
}

/////////////////////////////////////////////////////////

static bool TryParseHex(char hexChar, int& value)
{
	if (hexChar >= '0' && hexChar <= '9')
	{
		value = hexChar - '0';
		return true;
	}
	if (hexChar >= 'a' && hexChar <= 'f')
	{
		value = hexChar - 'a' + 10;
		return true;
	}
	if (hexChar >= 'A' && hexChar <= 'F')
	{
		value = hexChar - 'A' + 10;
		return true;
	}
	return false;
}

static bool TryParseColor(std::string& cssColorStr, SkColor& color)
{
	cssColorStr.erase(std::remove_if(cssColorStr.begin(), cssColorStr.end(), isspace), cssColorStr.end());
	color = 0xFF000000;
	if (cssColorStr[0] == '#')
	{
		if (cssColorStr.length() == 4)
		{
			int C[3]; // blue, green, red
			for (int i = 0; i < 3; ++i)
			{
				if (!TryParseHex(cssColorStr[i + 1], C[2 - i]))
					return false;
			}
			for (int i = 0; i < 3; ++i)
			{
				color |= C[i] << (8 * i);
				color |= C[i] << (8 * i + 4);
			}
			return true;
		}
		else if (cssColorStr.length() == 7)
		{
			int C[6]; // blue x2, green x2, red x2
			for (int i = 0; i < 6; ++i)
			{
				if (!TryParseHex(cssColorStr[i + 1], C[5 - i]))
					return false;
			}
			for (int i = 0; i < 6; ++i)
				color |= C[i] << (4 * i);
			return true;
		}
	}
	else
	{
		int C[3];
		float alpha = 1;
		if (sscanf(cssColorStr.c_str(), "rgb(%d,%d,%d)", &C[2], &C[1], &C[0]) == 3
			|| sscanf(cssColorStr.c_str(), "rgba(%d,%d,%d,%f)", &C[2], &C[1], &C[0], &alpha) == 4)
		{
			if (alpha != 1)
			{
				if (alpha < 0)
					alpha = 0;
				if (alpha > 1)
					alpha = 1;
				color = ((int) round(alpha * 255)) << 24;
			}
			for (int i = 0; i < 3; ++i)
				color |= C[i] << (8 * i);
			return true;
		}
	}
	return false;
}

void CanvasRenderingContext2D::GetFillStyle(Local<Name> property, const PropertyCallbackInfo<Value>& info)
{
	auto context = ToContext(info);
	info.GetReturnValue().Set(ConvertToV8String(context->myFillStyle));
}

void CanvasRenderingContext2D::SetFillStyle(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info)
{
	if (!value->IsString())
		return;

	auto fillStyle = ConvertToString(value->ToString());
	SkColor fillColor;
	if (!TryParseColor(fillStyle, fillColor))
		return;

	auto context = ToContext(info);
	context->myFillStyle = fillStyle;
	context->myFillColor = fillColor;
}

void CanvasRenderingContext2D::GetStrokeStyle(Local<Name> property, const PropertyCallbackInfo<Value>& info)
{
	auto context = ToContext(info);
	info.GetReturnValue().Set(ConvertToV8String(context->myStrokeStyle));
}

void CanvasRenderingContext2D::SetStrokeStyle(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info)
{
	if (!value->IsString())
		return;

	auto strokeStyle = ConvertToString(value->ToString());
	SkColor strokeColor;
	if (!TryParseColor(strokeStyle, strokeColor))
		return;

	auto context = ToContext(info);
	context->myStrokeStyle = strokeStyle;
	context->myStrokeColor = strokeColor;
}

////////////////////////////////////////////////////////////////

void CanvasRenderingContext2D::Arc(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	//void ctx.arc(x, y, radius, startAngle, endAngle, anticlockwise);
	auto context = ToContext(args);
	auto x = args[0]->NumberValue();
	auto y = args[1]->NumberValue();
	auto radius = args[2]->NumberValue();
	auto startAngle = DegFromRad(args[3]->NumberValue());
	auto endAngle = DegFromRad(args[4]->NumberValue());
	auto anticlockwise = args[5]->BooleanValue();

	endAngle -= 0.0001;
	auto rect = SkRect::MakeLTRB(
			SkDoubleToScalar(x - radius),
			SkDoubleToScalar(y - radius),
			SkDoubleToScalar(x + radius),
			SkDoubleToScalar(y + radius));
	auto sweepAngle = endAngle - startAngle;
	if (anticlockwise) {
		sweepAngle -= 360;
	}
	context->myPath->arcTo(rect, SkDoubleToScalar(startAngle), SkDoubleToScalar(sweepAngle), false);
}

void CanvasRenderingContext2D::BeginPath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	auto context = ToContext(args);
	if (context->myPath)
		delete context->myPath;
	context->myPath = new SkPath();
}

void CanvasRenderingContext2D::BezierCurveTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
//	void ctx.bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y);
	auto context = ToContext(args);
	auto cp1x = SkDoubleToScalar(args[0]->NumberValue());
	auto cp1y = SkDoubleToScalar(args[1]->NumberValue());
	auto cp2x = SkDoubleToScalar(args[2]->NumberValue());
	auto cp2y = SkDoubleToScalar(args[3]->NumberValue());
	auto x = SkDoubleToScalar(args[4]->NumberValue());
	auto y = SkDoubleToScalar(args[5]->NumberValue());
	context->myPath->cubicTo(cp1x, cp1y, cp2x, cp2y, x, y);
}

void CanvasRenderingContext2D::ClearRect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
//	void ctx.clearRect(x, y, width, height);
	auto x = SkDoubleToScalar(args[0]->NumberValue());
	auto y = SkDoubleToScalar(args[1]->NumberValue());
	auto width = SkDoubleToScalar(args[2]->NumberValue());
	auto height = SkDoubleToScalar(args[3]->NumberValue());

	auto context = ToContext(args);
	context->myCanvas->drawRect(SkRect::MakeXYWH(x, y, width, height), *context->myClearPaint);
}

void CanvasRenderingContext2D::ClosePath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	auto context = ToContext(args);
	context->myPath->close();
}

void CanvasRenderingContext2D::DrawImage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	//TODO: unimplemented
}

void CanvasRenderingContext2D::Fill(const v8::FunctionCallbackInfo<v8::Value>& args)
{
//	void ctx.fill();
//	void ctx.fill(fillRule);
//	void ctx.fill(path, fillRule);
	auto context = ToContext(args);

	//TODO: fill rules
//	auto path = args[0];
//	auto fillRule = args[1];
//	if (fillRule->IsUndefined())
//	{
//		fillRule = args[0];
//		path
//	}

	context->myPaint->setStyle(SkPaint::kFill_Style);
	context->myPaint->setColor(context->myFillColor);
	context->myCanvas->drawPath(*context->myPath, *context->myPaint);
}

void CanvasRenderingContext2D::FillRect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	auto x = SkDoubleToScalar(args[0]->NumberValue());
	auto y = SkDoubleToScalar(args[1]->NumberValue());
	auto width = SkDoubleToScalar(args[2]->NumberValue());
	auto height = SkDoubleToScalar(args[3]->NumberValue());

	auto context = ToContext(args);
	context->myPaint->setStyle(SkPaint::kFill_Style);
	context->myPaint->setColor(context->myFillColor);
	context->myCanvas->drawRect(SkRect::MakeXYWH(x, y, width, height), *context->myPaint);
}

void CanvasRenderingContext2D::FillText(const v8::FunctionCallbackInfo<v8::Value>& args)
{
//	void ctx.fillText(text, x, y [, maxWidth]);
	String::Utf8Value text(args[0]->ToString());
	auto x = SkDoubleToScalar(args[1]->NumberValue());
	auto y = SkDoubleToScalar(args[2]->NumberValue());
	auto maxWidth = args[3];

	auto context = ToContext(args);
	context->myPaint->setStyle(SkPaint::kFill_Style);
	context->myPaint->setColor(context->myFillColor);
	context->myCanvas->drawText(*text, text.length(), x, y, *context->myPaint);
}

void CanvasRenderingContext2D::GetImageData(const v8::FunctionCallbackInfo<v8::Value>& args)
{
//	ImageData ctx.getImageData(sx, sy, sw, sh);
// TODO:
//	IndexSizeError
//	    Thrown if either of the width or height arguments are zero.
	auto context = ToContext(args);
	auto isolate = args.GetIsolate();
	auto imageData = Object::New(isolate);

	auto x = args[0]->Int32Value();
	auto y = args[1]->Int32Value();
	auto width = args[2]->Int32Value();
	auto height = args[3]->Int32Value();
	width = std::min(context->myWidth - x, width);
	height = std::min(context->myHeight - y, height);

	imageData->ForceSet(ConvertToV8String("width"), Number::New(isolate, width));
	imageData->ForceSet(ConvertToV8String("height"), Number::New(isolate, height));

	int srcRowLength = context->myWidth * 4;
	int rowLength = width * 4;
	int dataLength = height * rowLength;
	char* data = new char[dataLength];

	char* destPtr = data;
	char* sourcePtr = &context->myPixelMemory[y * srcRowLength + x * 4];
	for (int hi = 0; hi < height; ++hi, destPtr += rowLength, sourcePtr += srcRowLength)
	{
		memcpy(destPtr, sourcePtr, rowLength);
	}

	Local<ArrayBuffer> buffer = ArrayBuffer::New(isolate, data, dataLength);
	imageData->ForceSet(ConvertToV8String("data"), Uint8ClampedArray::New(buffer, 0, dataLength));

	Persistent<ArrayBuffer> persistentHandle(isolate, buffer);
	persistentHandle.SetWeak(data, &DeallocateBuffer);

	args.GetReturnValue().Set(imageData);
}

void CanvasRenderingContext2D::LineTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	auto context = ToContext(args);
	auto x = SkDoubleToScalar(args[0]->NumberValue());
	auto y = SkDoubleToScalar(args[1]->NumberValue());
	context->myPath->lineTo(x, y);
}

void CanvasRenderingContext2D::MeasureText(const v8::FunctionCallbackInfo<v8::Value>& args)
{
//	ctx.measureText(text);

	auto context = ToContext(args);
	String::Utf8Value text(args[0]->ToString());
	auto width = SkScalarToDouble(context->myPaint->measureText(*text, text.length()));
	auto result = Object::New(args.GetIsolate());
	result->ForceSet(ConvertToV8String("width"), Number::New(args.GetIsolate(), width));
	args.GetReturnValue().Set(result);
}

void CanvasRenderingContext2D::MoveTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	auto context = ToContext(args);
	auto x = SkDoubleToScalar(args[0]->NumberValue());
	auto y = SkDoubleToScalar(args[1]->NumberValue());
	context->myPath->moveTo(x, y);
}

void CanvasRenderingContext2D::QuadraticCurveTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
//	void ctx.quadraticCurveTo(cpx, cpy, x, y);
	auto context = ToContext(args);
	auto cpx = SkDoubleToScalar(args[0]->NumberValue());
	auto cpy = SkDoubleToScalar(args[1]->NumberValue());
	auto x = SkDoubleToScalar(args[2]->NumberValue());
	auto y = SkDoubleToScalar(args[3]->NumberValue());
	context->myPath->quadTo(cpx, cpy, x, y);
}

void CanvasRenderingContext2D::Restore(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	auto context = ToContext(args);
	if (context->myPaintStack.size() > 0)
	{
		delete context->myPaint;
		context->myPaint = context->myPaintStack.back();
		context->myPaintStack.pop_back();
		context->myCanvas->restore();
	}
}

void CanvasRenderingContext2D::Rotate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
//	void ctx.rotate(angle);
	auto context = ToContext(args);
	auto angle = DegFromRad(args[0]->NumberValue());
	context->myCanvas->rotate(SkDoubleToScalar(angle));
}

void CanvasRenderingContext2D::Save(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	auto context = ToContext(args);
	auto savedPaint = new SkPaint(*context->myPaint);
	context->myPaintStack.push_back(savedPaint);
	context->myCanvas->save();
}

void CanvasRenderingContext2D::Stroke(const v8::FunctionCallbackInfo<v8::Value>& args)
{
//	void ctx.stroke();
//	void ctx.stroke(path);
	auto context = ToContext(args);
	context->myPaint->setStyle(SkPaint::kStroke_Style);
	context->myPaint->setColor(context->myStrokeColor);
	context->myCanvas->drawPath(*context->myPath, *context->myPaint);
}

void CanvasRenderingContext2D::Translate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
//	void ctx.translate(x, y);
	auto context = ToContext(args);
	auto x = SkDoubleToScalar(args[0]->NumberValue());
	auto y = SkDoubleToScalar(args[1]->NumberValue());
	context->myCanvas->translate(x, y);
}

///////////////////////////////////////////////////////////

extern "C" jlong Java_com_tns_CanvasViewBase_lockBitmap(JNIEnv* env, jobject _this, jobject dstBitmap)
{
	void* dstPixels;
	AndroidBitmap_lockPixels(env, dstBitmap, &dstPixels);
	return jlong(dstPixels);
}

extern "C" jlong Java_com_tns_CanvasViewBase_unlockBitmap(JNIEnv* env, jobject _this, jobject dstBitmap)
{
	AndroidBitmap_unlockPixels(env, dstBitmap);
}

