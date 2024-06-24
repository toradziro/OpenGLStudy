#pragma once

#include <GL/glew.h>

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