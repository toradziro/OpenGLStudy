#pragma once

#include <string>
#include <unordered_map>

class Shader
{
public:
	struct ShaderProgramSource
	{
		std::string m_vertexSource;
		std::string m_fragmentSource;
	};

	using UniformCache = std::unordered_map<std::string, int>;

	Shader(const std::string& path);
	~Shader();

	void bind() const;
	void unbind() const;

	//-- Set uniforms
	void setUniform4f(const std::string& name, float v0, float v1, float v2, float v4);
	void setUniform1f(const std::string& name, float v0);
	void setUniform1i(const std::string& name, int v0);

private:
	uint32_t			uniformLocation(const std::string& name);
	uint32_t			compileShader(int type, const std::string& srcStr);
	ShaderProgramSource	parseShader();
	void				createShader(const std::string& vertexShader, const std::string& fragmentShader);

	//-- Uniform cache
	UniformCache		m_uniformCache;

	std::string			m_filePath;
	uint32_t			m_rendererId;
};