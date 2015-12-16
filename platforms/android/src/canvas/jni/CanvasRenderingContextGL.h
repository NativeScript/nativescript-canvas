/*
 * CanvasRenderingContextGL.h
 *
 *  Created on: Dec 16, 2015
 *      Author: dragnev
 */

#ifndef CANVASRENDERINGCONTEXTGL_H_
#define CANVASRENDERINGCONTEXTGL_H_

#include "v8.h"

class CanvasRenderingContextGL {
public:
	CanvasRenderingContextGL();
	~CanvasRenderingContextGL();

	static void Create(const v8::FunctionCallbackInfo<v8::Value>& args);

private:
};

#endif /* CANVASRENDERINGCONTEXTGL_H_ */
