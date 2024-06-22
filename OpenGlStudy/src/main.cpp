#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

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
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	//-- Compiling errors handling
	int res;
	glGetShaderiv(id, GL_COMPILE_STATUS, &res);
	
	if (res == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		
		char* msg = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, msg);
		
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
		std::cout << msg << std::endl;
		
		glDeleteShader(id);
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
	glAttachShader(programIndex, vertexShaderIndex);
	glAttachShader(programIndex, fragmentShaderIndex);
	//-- Link shaders together
	glLinkProgram(programIndex);
	//-- Validate if linking went well
	glValidateProgram(programIndex);

	//-- Delete intermediate files generated during compilation
	glDeleteShader(vertexShaderIndex);
	glDeleteShader(fragmentShaderIndex);

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
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	
	//-- Make the window's context current
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		std::cerr << __FUNCTION__ << " " << __LINE__ << std::endl;
		return -1;
	}
	
	std::cout << "GLFW version: " << glfwGetVersionString() << std::endl;
	std::cout << "GLEW version: " << glGetString(GL_VERSION) << std::endl;

	float positions[] =
	{
		-0.5f,	-0.5f,
		0.0f,	0.5f,
		0.5f,	-0.5f
	};

	uint32_t buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	glEnableVertexAttribArray(0);

	//-- Parse shader files
	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

	//-- Vertex shader will be called for every vertex in array
	//-- gl_Position is vec4 so we consider in as vec4 just to avoid casting from vec2
	uint32_t shaderProg = createShader(source.m_vertexSource, source.m_fragmentSource);
	//-- Bind our shader to our vertex array
	glUseProgram(shaderProg);

	//-- Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		//-- Render here
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		//-- Swap front and back buffers
		glfwSwapBuffers(window);
		
		//-- Poll for and process events
		glfwPollEvents();
	}

	glDeleteProgram(shaderProg);

	glfwTerminate();
	return 0;
}