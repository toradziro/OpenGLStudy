#include "Renderer.h"
#include <iostream>

void GLCLearError()
{
	while (glGetError() != GL_NO_ERROR) {}
}

bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		if (error == GL_NO_ERROR)
		{
			break;
		}
		std::cout << "[OpenGL Error] (" << error << "): " <<
			function << " " << file << " " << line << std::endl;
		return false;
	}
	return true;
}