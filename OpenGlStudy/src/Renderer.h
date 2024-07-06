#pragma once

#include <GL/glew.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#if defined(_MSC_VER)
	#define DEBUG_BREAK() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
	#if defined(__i386__) || defined(__x86_64__)
		#define DEBUG_BREAK() asm volatile("int3")
	#else
		#include <csignal>
		#define DEBUG_BREAK() raise(SIGTRAP)
	#endif
#elif defined(__MINGW32__) || defined(__MINGW64__)
	#define DEBUG_BREAK() __builtin_trap()
#else
	#include <cassert>
	#define DEBUG_BREAK() assert(false)
#endif

#define ASSERT(x) if (!(x)) { DEBUG_BREAK(); }
#define GLCall(x) GLCLearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLCLearError();

bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
public:
	//-- To draw we at least need vertex array, vertex buffer, index buffer, shaders and uniforms
	//-- at lest it's what we know so far
	//-- but as we remember vertex buffer is bound to vertex array as layout is so vertex buffer isn't really what we need
	void draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader) const;

	void clear() const;
	void setClearColor(const float rgba[4]);
};