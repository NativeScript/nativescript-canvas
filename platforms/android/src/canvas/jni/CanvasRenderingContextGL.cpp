#include "CanvasRenderingContextGL.h"
#include "V8GlobalHelpers.h"
#include <GLES2/gl2.h>
#include <android/log.h>
#include <string>
#include <vector>
#include <ctype.h>

using namespace v8;

#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, "NGL", __VA_ARGS__)

CanvasRenderingContextGL::CanvasRenderingContextGL()
{
}

CanvasRenderingContextGL::~CanvasRenderingContextGL()
{
}

#define NOP(x)
#define B(x) args[x]->BooleanValue()
#define F(x) args[x]->NumberValue()
#define I(x) args[x]->Int32Value()
#define S(x) *String::Utf8Value(args[x]->ToString())
#define V(x) x; if (glGetError() != GL_NO_ERROR) args.GetIsolate()->ThrowException(ConvertToV8String("glGetError says no go"))
#define R(x) args.GetReturnValue().Set(x); if (glGetError() != GL_NO_ERROR) args.GetIsolate()->ThrowException(ConvertToV8String("glGetError says no go"))
#define L(x) x NOP
#define FV(x) (const GLfloat*)(args[x].As<Float32Array>()->Buffer()->GetContents().Data())
#define IV(x) (const GLint*)(args[x].As<Int32Array>()->Buffer()->GetContents().Data())

struct RegistrationData
{
	std::string Name;
	FunctionCallback Impl;
};
static std::vector<RegistrationData> registrations;

struct Registration
{
	Registration(const std::string& name, FunctionCallback impl)
	{
		RegistrationData data = {name, impl};
		data.Name[0] = tolower(data.Name[0]);
		registrations.push_back(data);
	}
};

#define REG(name) Registration _reg##name(#name, &js##name);
#define WRAP0(name, RF) \
	void js##name(const FunctionCallbackInfo<Value>& args) \
	{ \
		LOGI("gl"#name"()"); \
		RF(gl##name()); \
	} REG(name)
#define WRAP1(name, T0, RF) \
	void js##name(const FunctionCallbackInfo<Value>& args) \
	{ \
		LOGI("gl"#name"(%s)", S(0)); \
		RF(gl##name(T0(0))); \
	} REG(name)
#define WRAP2(name, T0, T1, RF) \
	void js##name(const FunctionCallbackInfo<Value>& args) \
	{ \
		LOGI("gl"#name"(%s, %s)", S(0), S(1)); \
		RF(gl##name(T0(0), T1(1))); \
	} REG(name)
#define WRAP3(name, T0, T1, T2, RF) \
	void js##name(const FunctionCallbackInfo<Value>& args) \
	{ \
		LOGI("gl"#name"(%s, %s, %s)", S(0), S(1), S(2)); \
		RF(gl##name(T0(0), T1(1), T2(2))); \
	} REG(name)
#define WRAP4(name, T0, T1, T2, T3, RF) \
	void js##name(const FunctionCallbackInfo<Value>& args) \
	{ \
		LOGI("gl"#name"(%s, %s, %s, %s)", S(0), S(1), S(2), S(3)); \
		RF(gl##name(T0(0), T1(1), T2(2), T3(3))); \
	} REG(name)
#define WRAP5(name, T0, T1, T2, T3, T4, RF) \
	void js##name(const FunctionCallbackInfo<Value>& args) \
	{ \
		LOGI("gl"#name"(%s, %s, %s, %s, %s)", S(0), S(1), S(2), S(3), S(4)); \
		RF(gl##name(T0(0), T1(1), T2(2), T3(3), T4(4))); \
	} REG(name)
#define WRAP6(name, T0, T1, T2, T3, T4, T5, RF) \
	void js##name(const FunctionCallbackInfo<Value>& args) \
	{ \
		LOGI("gl"#name"(%s, %s, %s, %s, %s, %s)", S(0), S(1), S(2), S(3), S(4), S(5)); \
		RF(gl##name(T0(0), T1(1), T2(2), T3(3), T4(4), T5(5))); \
	} REG(name)

#define RESOURCE_CTOR(name) \
	void jsCreate##name(const FunctionCallbackInfo<Value>& args) \
	{ \
		GLuint buf; \
		LOGI("glGen"#name"()"); \
		glGen##name##s(1, &buf); \
		args.GetReturnValue().Set(buf); \
	} Registration _regCreate##name("Create"#name, &jsCreate##name); \
	void jsDelete##name(const FunctionCallbackInfo<Value>& args) \
	{ \
		GLuint buf = args[0]->Int32Value(); \
		LOGI("glDelete"#name"()"); \
		glDelete##name##s(1, &buf); \
	} Registration _regDelete##name("Delete"#name, &jsDelete##name); \
	WRAP1(Is##name, I, R)

//TODO Context information methods - getContextAttributes, isContextLost, etc.

WRAP4(Scissor, I, I, I, I, V)
WRAP4(Viewport, I, I, I, I, V)

WRAP1(ActiveTexture, I, V)
WRAP4(BlendColor, F, F, F, F, V)
WRAP1(BlendEquation, I, V)
WRAP2(BlendEquationSeparate, I, I, V)
WRAP2(BlendFunc, I, I, V)
WRAP4(BlendFuncSeparate, I, I, I, I, V)
WRAP4(ClearColor, F, F, F, F, V)
WRAP1(ClearDepthf, F, V)
WRAP1(ClearStencil, I, V)
WRAP4(ColorMask, B, B, B, B, V)
WRAP1(CullFace, I, V)
WRAP1(DepthFunc, I, V)
WRAP1(DepthMask, B, V)
WRAP2(DepthRangef, F, F, V)
WRAP1(Disable, I, V)
WRAP1(Enable, I, V)
WRAP1(FrontFace, I, V)
//TODO: getParameter
WRAP0(GetError, R)
WRAP2(Hint, I, I, V)
WRAP1(IsEnabled, I, R)
WRAP1(LineWidth, F, V)
WRAP2(PixelStorei, I, I, V)
WRAP2(PolygonOffset, F, F, V)
WRAP2(SampleCoverage, F, B, V)
WRAP3(StencilFunc, I, I, I, V)
WRAP4(StencilFuncSeparate, I, I, I, I, V)
WRAP1(StencilMask, I, V)
WRAP2(StencilMaskSeparate, I, I, V)
WRAP3(StencilOp, I, I, I, V)
WRAP4(StencilOpSeparate, I, I, I, I, V)
/////////////////////
WRAP2(BindBuffer, I, I, V)

void jsBufferData(const FunctionCallbackInfo<Value>& args)
{
	auto contents = args[1].As<ArrayBufferView>()->Buffer()->GetContents();
	LOGI("glBufferData(%d, %d, %d, %d)", I(0), contents.ByteLength(), contents.Data(), I(2));
	glBufferData(I(0), contents.ByteLength(), contents.Data(), I(2));
}
Registration _regjsBufferData("BufferData", &jsBufferData);

//TODO: bufferSubData
RESOURCE_CTOR(Buffer)
//TODO: getBufferParameter
WRAP6(VertexAttribPointer, I, I, I, B, I, L((void*) I(5)), V);

/////////////////////

WRAP2(BindFramebuffer, I, I, V)
WRAP1(CheckFramebufferStatus, I, R)
RESOURCE_CTOR(Framebuffer)
WRAP4(FramebufferRenderbuffer, I, I, I, I, V)
WRAP5(FramebufferTexture2D, I, I, I, I, I, V)
//TOD: getFramebufferAttachmentParameter
//TODO: readPixels

//////////////////////

WRAP2(BindRenderbuffer, I, I, V)
RESOURCE_CTOR(Renderbuffer)
//TODO: getRenderbufferParameter()
WRAP4(RenderbufferStorage, I, I, I, I, V);

///////////////////////
//TODO: all texture stuff
WRAP2(BindTexture, I, I, V)
RESOURCE_CTOR(Texture)

//////////////////////////
//TODO: all shader stuff
WRAP2(AttachShader, I, I, V)
WRAP3(BindAttribLocation, I, I, S, V)
WRAP1(CompileShader, I, V)
WRAP0(CreateProgram, R)
WRAP1(CreateShader, I, R)
WRAP1(DeleteProgram, I, V)
WRAP1(DeleteShader, I, V)
WRAP2(DetachShader, I, I, V)
WRAP1(EnableVertexAttribArray, I, V)
WRAP2(GetAttribLocation, I, S, R)
WRAP2(GetUniformLocation, I, S, R)

WRAP2(Uniform1f, I, F, V)
WRAP3(Uniform2f, I, F, F, V)
WRAP4(Uniform3f, I, F, F, F, V)
WRAP5(Uniform4f, I, F, F, F, F, V)
WRAP2(Uniform1i, I, I, V)
WRAP3(Uniform2i, I, I, I, V)
WRAP4(Uniform3i, I, I, I, I, V)
WRAP5(Uniform4i, I, I, I, I, I, V)

#define MAKE_UNIFORM(JSType, GLtype, GLfunc, N) \
void js##GLfunc(const FunctionCallbackInfo<Value>& args) \
{ \
	auto buffer = args[1].As<JSType>()->Buffer(); \
	auto contents = buffer->GetContents(); \
	gl##GLfunc(I(0), contents.ByteLength() / (N * sizeof(GLtype)), (const GLtype*)contents.Data()); \
} Registration _reg##GLfunc(#GLfunc, &js##GLfunc);

MAKE_UNIFORM(Float32Array, GLfloat, Uniform1fv, 1)
MAKE_UNIFORM(Float32Array, GLfloat, Uniform2fv, 2)
MAKE_UNIFORM(Float32Array, GLfloat, Uniform3fv, 3)
MAKE_UNIFORM(Float32Array, GLfloat, Uniform4fv, 4)
MAKE_UNIFORM(Int32Array, GLint, Uniform1iv, 1)
MAKE_UNIFORM(Int32Array, GLint, Uniform2iv, 2)
MAKE_UNIFORM(Int32Array, GLint, Uniform3iv, 3)
MAKE_UNIFORM(Int32Array, GLint, Uniform4iv, 4)
#undef MAKE_UNIFORM

#define MAKE_UNIFORM_MATRIX(GLfunc, N) \
void js##GLfunc(const FunctionCallbackInfo<Value>& args) \
{ \
	auto buffer = args[2].As<Float32Array>()->Buffer(); \
	auto contents = buffer->GetContents(); \
	gl##GLfunc(I(0), contents.ByteLength() / (N * N * sizeof(GLfloat)), B(1), (const GLfloat*)contents.Data()); \
} Registration _reg##GLfunc(#GLfunc, &js##GLfunc);

MAKE_UNIFORM_MATRIX(UniformMatrix2fv, 2)
MAKE_UNIFORM_MATRIX(UniformMatrix3fv, 3)
MAKE_UNIFORM_MATRIX(UniformMatrix4fv, 4)
#undef MAKE_UNIFORM_MATRIX

void jsGetShaderInfoLog(const FunctionCallbackInfo<Value>& args)
{
	GLint logLength = 0;
	glGetShaderiv(I(0), GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> log;
	log.resize(logLength);
	glGetShaderInfoLog(I(0), logLength, NULL, &log[0]);
	R(ConvertToV8String(&log[0]));
}
Registration _regjsGetShaderInfoLog("GetShaderInfoLog", &jsGetShaderInfoLog);

void jsGetShaderParameter(const FunctionCallbackInfo<Value>& args)
{
	GLint result;
	glGetShaderiv(I(0), I(1), &result);
	R(result);
}
Registration _regjsGetShaderParameter("GetShaderParameter", &jsGetShaderParameter);

void jsGetProgramParameter(const FunctionCallbackInfo<Value>& args)
{
	GLint result;
	glGetProgramiv(I(0), I(1), &result);
	R(result);
}
Registration _regjsGetProgramParameter("GetProgramParameter", &jsGetProgramParameter);

WRAP1(IsProgram, I, R)
WRAP1(IsShader, I, R)
WRAP1(LinkProgram, I, V)

void jsShaderSource(const FunctionCallbackInfo<Value>& args)
{
	String::Utf8Value program(args[1]);
	const char* programCstr = *program;
	glShaderSource(args[0]->Int32Value(), 1, &programCstr, NULL);
}
Registration _regjsShaderSource("ShaderSource", &jsShaderSource);

WRAP1(UseProgram, I, V)
WRAP1(ValidateProgram, I, V)

//////////////////////
WRAP1(Clear, I, V)
WRAP3(DrawArrays, I, I, I, V)
WRAP0(Finish, V)
WRAP0(Flush, V)
//TODO: drawElements

/////////////
//TODO: getSupportedExtensions
//TODO: getExtension
//////////////

#undef B
#undef F
#undef I
#undef V
#undef R
#undef WRAP0
#undef WRAP1
#undef WRAP2
#undef WRAP4
#undef WRAP5
#undef WRAP6
#undef RESOURCE_CTOR

void Export(Isolate* isolate, Local<ObjectTemplate>& templ, const char* name, FunctionCallback impl)
{
	return templ->Set(ConvertToV8String(name), FunctionTemplate::New(isolate, impl));
}

void CanvasRenderingContextGL::Create(const FunctionCallbackInfo<Value>& args)
{
	auto isolate = args.GetIsolate();
	HandleScope scope(isolate);

	Local<ObjectTemplate> contextTemplate = ObjectTemplate::New(isolate);

	for (auto& reg : registrations)
		Export(isolate, contextTemplate, reg.Name.c_str(), reg.Impl);
	std::vector<RegistrationData>().swap(registrations);

	Local<Object> newContext = contextTemplate->NewInstance();
	newContext->ForceSet(ConvertToV8String("canvas"), args[0]);
	newContext->ForceSet(ConvertToV8String("__kind"), ConvertToV8String("webgl"));

#define SET_CONSTANT(C) \
	newContext->ForceSet(ConvertToV8String(#C), Int32::New(isolate, GL_##C));

	SET_CONSTANT(COLOR_BUFFER_BIT);
	SET_CONSTANT(DEPTH_BUFFER_BIT);
	SET_CONSTANT(STENCIL_BUFFER_BIT);

	SET_CONSTANT(POINTS);
	SET_CONSTANT(LINES);
	SET_CONSTANT(LINE_LOOP);
	SET_CONSTANT(LINE_STRIP);
	SET_CONSTANT(TRIANGLES);
	SET_CONSTANT(TRIANGLE_STRIP);
	SET_CONSTANT(TRIANGLE_FAN);

	SET_CONSTANT(ZERO);
	SET_CONSTANT(ONE);
	SET_CONSTANT(SRC_COLOR);
	SET_CONSTANT(ONE_MINUS_SRC_COLOR);
	SET_CONSTANT(SRC_ALPHA);
	SET_CONSTANT(ONE_MINUS_SRC_ALPHA);
	SET_CONSTANT(DST_ALPHA);
	SET_CONSTANT(ONE_MINUS_DST_ALPHA);
	SET_CONSTANT(DST_COLOR);
	SET_CONSTANT(ONE_MINUS_DST_COLOR);
	SET_CONSTANT(SRC_ALPHA_SATURATE);
	SET_CONSTANT(CONSTANT_COLOR);
	SET_CONSTANT(ONE_MINUS_CONSTANT_COLOR);
	SET_CONSTANT(CONSTANT_ALPHA);
	SET_CONSTANT(ONE_MINUS_CONSTANT_ALPHA);


	SET_CONSTANT(FUNC_ADD);
	SET_CONSTANT(FUNC_SUBTRACT);
	SET_CONSTANT(FUNC_REVERSE_SUBTRACT);

	SET_CONSTANT(BLEND_EQUATION);
	SET_CONSTANT(BLEND_EQUATION_RGB);
	SET_CONSTANT(BLEND_EQUATION_ALPHA);
	SET_CONSTANT(BLEND_DST_RGB);
	SET_CONSTANT(BLEND_SRC_RGB);
	SET_CONSTANT(BLEND_DST_ALPHA);
	SET_CONSTANT(BLEND_SRC_ALPHA);
	SET_CONSTANT(BLEND_COLOR);
	SET_CONSTANT(ARRAY_BUFFER_BINDING);
	SET_CONSTANT(ELEMENT_ARRAY_BUFFER_BINDING);
	SET_CONSTANT(LINE_WIDTH);
	SET_CONSTANT(ALIASED_POINT_SIZE_RANGE);
	SET_CONSTANT(ALIASED_LINE_WIDTH_RANGE);
	SET_CONSTANT(CULL_FACE_MODE);
	SET_CONSTANT(FRONT_FACE);
	SET_CONSTANT(DEPTH_RANGE);
	SET_CONSTANT(DEPTH_WRITEMASK);
	SET_CONSTANT(DEPTH_CLEAR_VALUE);
	SET_CONSTANT(DEPTH_FUNC);
	SET_CONSTANT(STENCIL_CLEAR_VALUE);
	SET_CONSTANT(STENCIL_FUNC);
	SET_CONSTANT(STENCIL_FAIL);
	SET_CONSTANT(STENCIL_PASS_DEPTH_FAIL);
	SET_CONSTANT(STENCIL_PASS_DEPTH_PASS);
	SET_CONSTANT(STENCIL_REF);
	SET_CONSTANT(STENCIL_VALUE_MASK);
	SET_CONSTANT(STENCIL_WRITEMASK);
	SET_CONSTANT(STENCIL_BACK_FUNC);
	SET_CONSTANT(STENCIL_BACK_FAIL);
	SET_CONSTANT(STENCIL_BACK_PASS_DEPTH_FAIL);
	SET_CONSTANT(STENCIL_BACK_PASS_DEPTH_PASS);
	SET_CONSTANT(STENCIL_BACK_REF);
	SET_CONSTANT(STENCIL_BACK_VALUE_MASK);
	SET_CONSTANT(STENCIL_BACK_WRITEMASK);
	SET_CONSTANT(VIEWPORT);
	SET_CONSTANT(SCISSOR_BOX);
	SET_CONSTANT(COLOR_CLEAR_VALUE);
	SET_CONSTANT(COLOR_WRITEMASK);
	SET_CONSTANT(UNPACK_ALIGNMENT);
	SET_CONSTANT(PACK_ALIGNMENT);
	SET_CONSTANT(MAX_TEXTURE_SIZE);
	SET_CONSTANT(MAX_VIEWPORT_DIMS);
	SET_CONSTANT(SUBPIXEL_BITS);
	SET_CONSTANT(RED_BITS);
	SET_CONSTANT(GREEN_BITS);
	SET_CONSTANT(BLUE_BITS);
	SET_CONSTANT(ALPHA_BITS);
	SET_CONSTANT(DEPTH_BITS);
	SET_CONSTANT(STENCIL_BITS);
	SET_CONSTANT(POLYGON_OFFSET_UNITS);
	SET_CONSTANT(POLYGON_OFFSET_FACTOR);
	SET_CONSTANT(TEXTURE_BINDING_2D);
	SET_CONSTANT(SAMPLE_BUFFERS);
	SET_CONSTANT(SAMPLES);
	SET_CONSTANT(SAMPLE_COVERAGE_VALUE);
	SET_CONSTANT(SAMPLE_COVERAGE_INVERT);
	SET_CONSTANT(COMPRESSED_TEXTURE_FORMATS);

	SET_CONSTANT(VENDOR);
	SET_CONSTANT(RENDERER);
	SET_CONSTANT(VERSION);
	SET_CONSTANT(IMPLEMENTATION_COLOR_READ_TYPE);
	SET_CONSTANT(IMPLEMENTATION_COLOR_READ_FORMAT);
	//SET_CONSTANT(BROWSER_DEFAULT_WEBGL);

	SET_CONSTANT(STATIC_DRAW);
	SET_CONSTANT(STREAM_DRAW);
	SET_CONSTANT(DYNAMIC_DRAW);
	SET_CONSTANT(ARRAY_BUFFER);
	SET_CONSTANT(ELEMENT_ARRAY_BUFFER);
	SET_CONSTANT(BUFFER_SIZE);
	SET_CONSTANT(BUFFER_USAGE);


	SET_CONSTANT(CURRENT_VERTEX_ATTRIB);
	SET_CONSTANT(VERTEX_ATTRIB_ARRAY_ENABLED);
	SET_CONSTANT(VERTEX_ATTRIB_ARRAY_SIZE);
	SET_CONSTANT(VERTEX_ATTRIB_ARRAY_STRIDE);
	SET_CONSTANT(VERTEX_ATTRIB_ARRAY_TYPE);
	SET_CONSTANT(VERTEX_ATTRIB_ARRAY_NORMALIZED);
	SET_CONSTANT(VERTEX_ATTRIB_ARRAY_POINTER);
	SET_CONSTANT(VERTEX_ATTRIB_ARRAY_BUFFER_BINDING);

	SET_CONSTANT(CULL_FACE);
	SET_CONSTANT(FRONT);
	SET_CONSTANT(BACK);
	SET_CONSTANT(FRONT_AND_BACK);

	SET_CONSTANT(BLEND);
	SET_CONSTANT(DEPTH_TEST);
	SET_CONSTANT(DITHER);
	SET_CONSTANT(POLYGON_OFFSET_FILL);
	SET_CONSTANT(SAMPLE_ALPHA_TO_COVERAGE);
	SET_CONSTANT(SAMPLE_COVERAGE);
	SET_CONSTANT(SCISSOR_TEST);
	SET_CONSTANT(STENCIL_TEST);

	SET_CONSTANT(NO_ERROR);
	SET_CONSTANT(INVALID_ENUM);
	SET_CONSTANT(INVALID_VALUE);
	SET_CONSTANT(INVALID_OPERATION);
	SET_CONSTANT(OUT_OF_MEMORY);
	//SET_CONSTANT(CONTEXT_LOST_WEBGL);

	SET_CONSTANT(CW);
	SET_CONSTANT(CCW);

	SET_CONSTANT(DONT_CARE);
	SET_CONSTANT(FASTEST);
	SET_CONSTANT(NICEST);
	SET_CONSTANT(GENERATE_MIPMAP_HINT);

	SET_CONSTANT(BYTE);
	SET_CONSTANT(UNSIGNED_BYTE);
	SET_CONSTANT(SHORT);
	SET_CONSTANT(UNSIGNED_SHORT);
	SET_CONSTANT(INT);
	SET_CONSTANT(UNSIGNED_INT);
	SET_CONSTANT(FLOAT);

	SET_CONSTANT(DEPTH_COMPONENT);
	SET_CONSTANT(ALPHA);
	SET_CONSTANT(RGB);
	SET_CONSTANT(RGBA);
	SET_CONSTANT(LUMINANCE);
	SET_CONSTANT(LUMINANCE_ALPHA);

	SET_CONSTANT(UNSIGNED_BYTE);
	SET_CONSTANT(UNSIGNED_SHORT_4_4_4_4);
	SET_CONSTANT(UNSIGNED_SHORT_5_5_5_1);
	SET_CONSTANT(UNSIGNED_SHORT_5_6_5);

	SET_CONSTANT(FRAGMENT_SHADER);
	SET_CONSTANT(VERTEX_SHADER);
	SET_CONSTANT(COMPILE_STATUS);
	SET_CONSTANT(DELETE_STATUS);
	SET_CONSTANT(LINK_STATUS);
	SET_CONSTANT(VALIDATE_STATUS);
	SET_CONSTANT(ATTACHED_SHADERS);
	SET_CONSTANT(ACTIVE_ATTRIBUTES);
	SET_CONSTANT(ACTIVE_UNIFORMS);
	SET_CONSTANT(MAX_VERTEX_ATTRIBS);
	SET_CONSTANT(MAX_VERTEX_UNIFORM_VECTORS);
	SET_CONSTANT(MAX_VARYING_VECTORS);
	SET_CONSTANT(MAX_COMBINED_TEXTURE_IMAGE_UNITS);
	SET_CONSTANT(MAX_VERTEX_TEXTURE_IMAGE_UNITS);
	SET_CONSTANT(MAX_TEXTURE_IMAGE_UNITS);
	SET_CONSTANT(MAX_FRAGMENT_UNIFORM_VECTORS);
	SET_CONSTANT(SHADER_TYPE);
	SET_CONSTANT(SHADING_LANGUAGE_VERSION);
	SET_CONSTANT(CURRENT_PROGRAM);

	SET_CONSTANT(NEVER);
	SET_CONSTANT(ALWAYS);
	SET_CONSTANT(LESS);
	SET_CONSTANT(EQUAL);
	SET_CONSTANT(LEQUAL);
	SET_CONSTANT(GREATER);
	SET_CONSTANT(GEQUAL);
	SET_CONSTANT(GEQUAL);

	SET_CONSTANT(KEEP);
	SET_CONSTANT(REPLACE);
	SET_CONSTANT(INCR);
	SET_CONSTANT(DECR);
	SET_CONSTANT(INVERT);
	SET_CONSTANT(INCR_WRAP);
	SET_CONSTANT(DECR_WRAP);

	SET_CONSTANT(NEAREST);
	SET_CONSTANT(LINEAR);
	SET_CONSTANT(NEAREST_MIPMAP_NEAREST);
	SET_CONSTANT(LINEAR_MIPMAP_NEAREST);
	SET_CONSTANT(NEAREST_MIPMAP_LINEAR);
	SET_CONSTANT(LINEAR_MIPMAP_LINEAR);
	SET_CONSTANT(TEXTURE_MAG_FILTER);
	SET_CONSTANT(TEXTURE_MIN_FILTER);
	SET_CONSTANT(TEXTURE_WRAP_S);
	SET_CONSTANT(TEXTURE_WRAP_T);
	SET_CONSTANT(TEXTURE_2D);
	SET_CONSTANT(TEXTURE);
	SET_CONSTANT(TEXTURE_CUBE_MAP);
	SET_CONSTANT(TEXTURE_BINDING_CUBE_MAP);
	SET_CONSTANT(TEXTURE_CUBE_MAP_POSITIVE_X);
	SET_CONSTANT(TEXTURE_CUBE_MAP_NEGATIVE_X);
	SET_CONSTANT(TEXTURE_CUBE_MAP_POSITIVE_Y);
	SET_CONSTANT(TEXTURE_CUBE_MAP_NEGATIVE_Y);
	SET_CONSTANT(TEXTURE_CUBE_MAP_POSITIVE_Z);
	SET_CONSTANT(TEXTURE_CUBE_MAP_NEGATIVE_Z);
	SET_CONSTANT(MAX_CUBE_MAP_TEXTURE_SIZE);
	SET_CONSTANT(ACTIVE_TEXTURE);
	SET_CONSTANT(REPEAT);
	SET_CONSTANT(CLAMP_TO_EDGE);
	SET_CONSTANT(MIRRORED_REPEAT);
	SET_CONSTANT(TEXTURE0);
	SET_CONSTANT(TEXTURE1);
	SET_CONSTANT(TEXTURE2);
	SET_CONSTANT(TEXTURE3);
	SET_CONSTANT(TEXTURE4);
	SET_CONSTANT(TEXTURE5);
	SET_CONSTANT(TEXTURE6);
	SET_CONSTANT(TEXTURE7);
	SET_CONSTANT(TEXTURE8);
	SET_CONSTANT(TEXTURE9);
	SET_CONSTANT(TEXTURE10);
	SET_CONSTANT(TEXTURE11);
	SET_CONSTANT(TEXTURE12);
	SET_CONSTANT(TEXTURE13);
	SET_CONSTANT(TEXTURE14);
	SET_CONSTANT(TEXTURE15);
	SET_CONSTANT(TEXTURE16);
	SET_CONSTANT(TEXTURE17);
	SET_CONSTANT(TEXTURE18);
	SET_CONSTANT(TEXTURE19);
	SET_CONSTANT(TEXTURE20);
	SET_CONSTANT(TEXTURE21);
	SET_CONSTANT(TEXTURE22);
	SET_CONSTANT(TEXTURE23);
	SET_CONSTANT(TEXTURE24);
	SET_CONSTANT(TEXTURE25);
	SET_CONSTANT(TEXTURE26);
	SET_CONSTANT(TEXTURE27);
	SET_CONSTANT(TEXTURE28);
	SET_CONSTANT(TEXTURE29);
	SET_CONSTANT(TEXTURE30);
	SET_CONSTANT(TEXTURE31);

	SET_CONSTANT(FLOAT_VEC2);
	SET_CONSTANT(FLOAT_VEC3);
	SET_CONSTANT(FLOAT_VEC4);
	SET_CONSTANT(INT_VEC2);
	SET_CONSTANT(INT_VEC3);
	SET_CONSTANT(INT_VEC4);
	SET_CONSTANT(BOOL);
	SET_CONSTANT(BOOL_VEC2);
	SET_CONSTANT(BOOL_VEC3);
	SET_CONSTANT(BOOL_VEC4);
	SET_CONSTANT(FLOAT_MAT2);
	SET_CONSTANT(FLOAT_MAT3);
	SET_CONSTANT(FLOAT_MAT4);
	SET_CONSTANT(SAMPLER_2D);
	SET_CONSTANT(SAMPLER_CUBE);

	SET_CONSTANT(LOW_FLOAT);
	SET_CONSTANT(MEDIUM_FLOAT);
	SET_CONSTANT(HIGH_FLOAT);
	SET_CONSTANT(LOW_INT);
	SET_CONSTANT(MEDIUM_INT);
	SET_CONSTANT(HIGH_INT);

	SET_CONSTANT(FRAMEBUFFER);
	SET_CONSTANT(RENDERBUFFER);
	SET_CONSTANT(RGBA4);
	SET_CONSTANT(RGB5_A1);
	SET_CONSTANT(RGB565);
	SET_CONSTANT(DEPTH_COMPONENT16);
	SET_CONSTANT(STENCIL_INDEX);
	SET_CONSTANT(STENCIL_INDEX8);
	//SET_CONSTANT(DEPTH_STENCIL);
	SET_CONSTANT(RENDERBUFFER_WIDTH);
	SET_CONSTANT(RENDERBUFFER_HEIGHT);
	SET_CONSTANT(RENDERBUFFER_INTERNAL_FORMAT);
	SET_CONSTANT(RENDERBUFFER_RED_SIZE);
	SET_CONSTANT(RENDERBUFFER_GREEN_SIZE);
	SET_CONSTANT(RENDERBUFFER_BLUE_SIZE);
	SET_CONSTANT(RENDERBUFFER_ALPHA_SIZE);
	SET_CONSTANT(RENDERBUFFER_DEPTH_SIZE);
	SET_CONSTANT(RENDERBUFFER_STENCIL_SIZE);
	SET_CONSTANT(FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE);
	SET_CONSTANT(FRAMEBUFFER_ATTACHMENT_OBJECT_NAME);
	SET_CONSTANT(FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL);
	SET_CONSTANT(FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE);
	SET_CONSTANT(COLOR_ATTACHMENT0);
	SET_CONSTANT(DEPTH_ATTACHMENT);
	SET_CONSTANT(STENCIL_ATTACHMENT);
	//SET_CONSTANT(DEPTH_STENCIL_ATTACHMENT);
	SET_CONSTANT(NONE);
	SET_CONSTANT(FRAMEBUFFER_COMPLETE);
	SET_CONSTANT(FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
	SET_CONSTANT(FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
	SET_CONSTANT(FRAMEBUFFER_INCOMPLETE_DIMENSIONS);
	SET_CONSTANT(FRAMEBUFFER_UNSUPPORTED);
	SET_CONSTANT(FRAMEBUFFER_BINDING);
	SET_CONSTANT(RENDERBUFFER_BINDING);
	SET_CONSTANT(MAX_RENDERBUFFER_SIZE);
	SET_CONSTANT(INVALID_FRAMEBUFFER_OPERATION);


	//SET_CONSTANT(UNPACK_FLIP_Y_WEBGL);
	//SET_CONSTANT(UNPACK_PREMULTIPLY_ALPHA_WEBGL);
	//SET_CONSTANT(UNPACK_COLORSPACE_CONVERSION_WEBGL);

	//TODO: WebGL2RenderingContext constants to follow

#undef SET_CONSTANT

	args.GetReturnValue().Set(newContext);
}
