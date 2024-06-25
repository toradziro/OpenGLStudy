#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"

struct ShaderProgramSource
{
	std::string m_vertexSource;
	std::string m_fragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& file)
{
	std::ifstream stream(file);

	enum class ShaderType
	{
		Vertex,
		Fragment,
		None
	};

	std::stringstream vertextStream;
	std::stringstream fragmentStream;
	
	ShaderType type;

	std::string line;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::Vertex;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::Fragment;
			}
		}
		else
		{
			if (type == ShaderType::Vertex)
			{
				vertextStream << line << '\n';
			}
			else if (type == ShaderType::Fragment)
			{
				fragmentStream << line << '\n';
			}
		}
	}
	return { vertextStream.str(), fragmentStream.str() };
}

static uint32_t compileShader(const std::string& source, uint32_t type)
{
	uint32_t id = glCreateShader(type);
	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	//-- Compiling errors handling
	int res;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &res));
	
	if (res == GL_FALSE)
	{
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		
		char* msg = (char*)alloca(length * sizeof(char));
		GLCall(glGetShaderInfoLog(id, length, &length, msg));
		
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
		std::cout << msg << std::endl;
		
		return 0;
	}

	return id;
}

//-- ret int will contain index to bind compiled shader to a vertex array
static uint32_t createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	uint32_t programIndex = glCreateProgram();
	uint32_t vertexShaderIndex = compileShader(vertexShader, GL_VERTEX_SHADER);
	uint32_t fragmentShaderIndex = compileShader(fragmentShader, GL_FRAGMENT_SHADER);

	//-- Attaching compiled in video card shader to program
	GLCall(glAttachShader(programIndex, vertexShaderIndex));
	GLCall(glAttachShader(programIndex, fragmentShaderIndex));
	//-- Link shaders together
	GLCall(glLinkProgram(programIndex));
	//-- Validate if linking went well
	GLCall(glValidateProgram(programIndex));

	//-- Delete intermediate files generated during compilation
	GLCall(glDeleteShader(vertexShaderIndex));
	GLCall(glDeleteShader(fragmentShaderIndex));

	return programIndex;
}

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
		-0.5f,	-0.5f, //-- vertex index 1
		0.5f,	-0.5f, //-- vertex index 2
		0.5f,	0.5f, //-- vertex index 3
		-0.5f,	0.5f, //-- vertex index 4
	};

	uint32_t indexBufferData[] =
	{
		0, 1, 2, //-- indices that describe first triangle for square
		2, 3, 0 //-- and second one
	};



	std::unique_ptr<VertexArray> vertexArray = std::make_unique<VertexArray>();
	std::unique_ptr<VertexBuffer> vertexBuffer = std::make_unique<VertexBuffer>(positions, uint32_t(2 * 4 * sizeof(float)));
	
	VertexBufferLayout layout;

	layout.push<float>(2);
	vertexArray->addBuffer(*vertexBuffer, layout);

	std::unique_ptr<IndexBuffer> indexBuffer = std::make_unique<IndexBuffer>(indexBufferData, 2 * 3);

	//-- Parse shader files
	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

	//-- Vertex shader will be called for every vertex in array
	//-- gl_Position is vec4 so we consider in as vec4 just to avoid casting from vec2
	uint32_t shaderProg = createShader(source.m_vertexSource, source.m_fragmentSource);

	//-- Bind our shader to our vertex array
	GLCall(glUseProgram(shaderProg));

	//-- Uniforms is our way to pass data from CPU to GPU, to some shader
	//-- Location is an shader index for uniform
	int location = glGetUniformLocation(shaderProg, "u_Color");
	ASSERT(location != -1);
	GLCall(glUniform4f(location, 0.0f, 0.2f, 0.5f, 1.0f));

	vertexBuffer->unbind();
	GLCall(glUseProgram(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	indexBuffer->unbind();

	float r = 0.0f;
	float inc = 0.005f;
	//-- Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		//-- Render here
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		GLCall(glUseProgram(shaderProg));
		GLCall(glUniform4f(location, r, 0.5f, 0.5f, 1.0f));

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

	GLCall(glDeleteProgram(shaderProg));

	vertexBuffer.reset();
	indexBuffer.reset();
	vertexArray.reset();

	glfwTerminate();
	return 0;
}