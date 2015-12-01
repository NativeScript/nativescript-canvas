/*
 * CanvasRenderingContext2D.h
 *
 *  Created on: Nov 12, 2015
 *      Author: dragnev
 */

#ifndef JNI_CANVASRENDERINGCONTEXT2D_H_
#define JNI_CANVASRENDERINGCONTEXT2D_H_

#include "v8.h"
#include "SkColor.h"
#include <vector>
#include <string>

class SkCanvas;
class SkSurface;
class SkPaint;
class SkPath;

class CanvasRenderingContext2D
{
public:
	CanvasRenderingContext2D();
	~CanvasRenderingContext2D();

	static void Create(const v8::FunctionCallbackInfo<v8::Value>& args);

private:
	std::string myFillStyle;
	std::string myStrokeStyle;

	SkColor myFillColor;
	SkColor myStrokeColor;

	SkSurface* mySurface;
	SkCanvas* myCanvas;
	SkPaint* myPaint;
	SkPath* myPath;
	SkPaint* myClearPaint;
	std::vector<SkPaint*> myPaintStack;

	std::vector<char> myPixelMemory;
	int myWidth;
	int myHeight;

	void CreateBackbuffer(int width, int height);
	static void Deallocate(const v8::WeakCallbackData<v8::Object, CanvasRenderingContext2D>& data);
	static void DeallocateBuffer(const v8::WeakCallbackData<v8::ArrayBuffer, char>& data);

	static void Draw(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void SizeChanged(const v8::FunctionCallbackInfo<v8::Value>& args);

	static void GetFillStyle(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	static void SetFillStyle(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
	static void GetStrokeStyle(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
	static void SetStrokeStyle(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);

	static void Arc(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void BeginPath(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void BezierCurveTo(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void ClearRect(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void ClosePath(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void DrawImage(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void Fill(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void FillRect(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void FillText(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void GetImageData(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void LineTo(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void MeasureText(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void MoveTo(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void QuadraticCurveTo(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void Restore(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void Rotate(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void Save(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void Stroke(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void Translate(const v8::FunctionCallbackInfo<v8::Value>& args);
};

#endif /* JNI_CANVASRENDERINGCONTEXT2D_H_ */
