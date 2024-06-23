#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <cmath>

#define PI 3.14159265358979323846

struct ShaderProgramSource
{
	std::string m_vertexSource;
	std::string m_fragmentSource;
};

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

static void GLCLearError()
{
	while (glGetError() != GL_NO_ERROR) { }
}

static bool GLLogCall(const char* function, const char* file, int line)
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

	uint32_t indexBuffer[] =
	{
		0, 1, 2, //-- indices that descride first triangle for square
		2, 3, 0 //-- and second one
	};

	uint32_t buffer;
	GLCall(glGenBuffers(1, &buffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
	GLCall(glBufferData(GL_ARRAY_BUFFER, 2 * 6 * sizeof(float), positions, GL_STATIC_DRAW));

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

	uint32_t indexBufferObject;
	GLCall(glGenBuffers(1, &indexBufferObject));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 3 * sizeof(uint32_t), indexBuffer, GL_STATIC_DRAW));
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

	float r = 0.0f;
	float inc = 0.005f;
	//-- Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		//-- Render here
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		GLCall(glUniform4f(location, r, 0.5f, 0.5f, 1.0f));
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

	glfwTerminate();
	return 0;
}