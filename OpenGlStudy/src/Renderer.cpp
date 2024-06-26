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

void Renderer::draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader) const
{
	shader.bind();
	vertexArray.bind();
	indexBuffer.bind();

	GLCall(glDrawElements(GL_TRIANGLES, indexBuffer.count(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::clear() const
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
}
