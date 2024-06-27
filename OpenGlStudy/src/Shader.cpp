#include "Shader.h"
#include "Renderer.h"

#include <sstream>
#include <fstream>
#include <iostream>

Shader::Shader(const std::string& path)
	: m_filePath(path), m_rendererId(0)
{
	auto shaderSrc = parseShader();
	createShader(shaderSrc.m_vertexSource, shaderSrc.m_fragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_rendererId));
}

void Shader::bind() const
{
	GLCall(glUseProgram(m_rendererId));
}

void Shader::unbind() const
{
	GLCall(glUseProgram(0));
}

Shader::ShaderProgramSource Shader::parseShader()
{
	std::ifstream stream(m_filePath);

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

uint32_t Shader::compileShader(int type, const std::string& srcStr)
{
	uint32_t id = glCreateShader(type);
	const char* src = srcStr.c_str();
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

void Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	uint32_t programIndex = glCreateProgram();
	uint32_t vertexShaderIndex = compileShader(GL_VERTEX_SHADER, vertexShader);
	uint32_t fragmentShaderIndex = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

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

	m_rendererId = programIndex;
}

void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(uniformLocation(name), v0, v1, v2, v3));
}


void Shader::setUniform1f(const std::string& name, float v0)
{
	GLCall(glUniform1f(uniformLocation(name), v0));
}

void Shader::setUniform1i(const std::string& name, int v0)
{
	GLCall(glUniform1i(uniformLocation(name), v0));
}

uint32_t Shader::uniformLocation(const std::string& name)
{
	//-- Uniforms is our way to pass data from CPU to GPU, to some shader
	//-- Location is an shader index for uniform
	if (m_uniformCache.count(name) == 0)
	{
		m_uniformCache[name] = glGetUniformLocation(m_rendererId, name.c_str());
	}

	if (m_uniformCache[name] == -1)
	{
		std::cerr << "[WARNING](OpenGl): Location == -1 for uniform name: '" <<  name << "'" << std::endl;
	}
	return m_uniformCache[name];
}
