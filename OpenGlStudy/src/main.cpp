#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"

int main(int argc, char** argv)
{
	GLFWwindow* window;
	
	//-- Initialize the library
	if (!glfwInit())
	{
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//-- Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(640, 640, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	
	//-- Make the window's context current
	glfwMakeContextCurrent(window);

	//-- Setting our framerate
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
	{
		std::cerr << __FUNCTION__ << " " << __LINE__ << std::endl;
		return -1;
	}
	
	std::cout << "GLFW version: " << glfwGetVersionString() << std::endl;
	std::cout << "GLEW version: " << glGetString(GL_VERSION) << std::endl;
	
	//-- square
	float positions[] =
	{
		-0.5f,	-0.5f,	//-- vertex index 1
		0.5f,	-0.5f,	//-- vertex index 2
		0.5f,	0.5f,	//-- vertex index 3
		-0.5f,	0.5f,	//-- vertex index 4
	};

	uint32_t indexBufferData[] =
	{
		0, 1, 2,	//-- indices that describe first triangle for square
		2, 3, 0		//-- and second one
	};

	std::unique_ptr<VertexArray> vertexArray = std::make_unique<VertexArray>();
	std::unique_ptr<VertexBuffer> vertexBuffer = std::make_unique<VertexBuffer>(positions, uint32_t(2 * 4 * sizeof(float)));
	
	VertexBufferLayout layout;

	layout.push<float>(2);
	vertexArray->addBuffer(*vertexBuffer, layout);

	std::unique_ptr<IndexBuffer> indexBuffer = std::make_unique<IndexBuffer>(indexBufferData, 2 * 3);

	std::unique_ptr<Shader> shader = std::make_unique<Shader>("res/shaders/Basic.shader");
	shader->bind();
	shader->setUniform4f("u_Color", 0.0f, 0.2f, 0.5f, 1.0f);

	vertexBuffer->unbind();
	indexBuffer->unbind();
	vertexArray->unbind();
	shader->unbind();

	float r = 0.0f;
	float inc = 0.005f;
	//-- Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		//-- Render here
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		shader->bind();
		shader->setUniform4f("u_Color", r, 0.2f, 0.5f, 1.0f);

		vertexArray->bind();
		indexBuffer->bind();

		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

		if (r > 1.0f)
		{
			inc = -inc;
		}
		else if (r < 0.0f)
		{
			inc = -inc;
		}

		r += inc;

		//-- Swap front and back buffers
		glfwSwapBuffers(window);
		
		//-- Poll for and process events
		glfwPollEvents();
	}

	shader.reset();
	vertexBuffer.reset();
	indexBuffer.reset();
	vertexArray.reset();

	glfwTerminate();
	return 0;
}