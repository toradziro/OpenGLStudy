#include "Texture.h"
#include "stb_image.h"

Texture::Texture(const std::string& path)
	: m_rendererId(0), m_filePath(path), m_localBuffer(nullptr), m_width(0), m_hieght(0), m_bitsPerPixel(0)
{
	//-- We need textures to be loaded upside down because OpenGL expects textures to start from bottom left, not top left
	stbi_set_flip_vertically_on_load(1);
	m_localBuffer = stbi_load(path.c_str(), &m_width, &m_hieght, &m_bitsPerPixel, 4 /*RGBA*/);

	GLCall(glGenTextures(1, &m_rendererId));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_rendererId));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_hieght, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_localBuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	
	if (m_localBuffer)
	{
		stbi_image_free(m_localBuffer);
	}
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_rendererId));
}

void Texture::bind(uint32_t slot /*= 0*/)
{
	//-- Which texture slot we want to use as current
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_rendererId));
}

void Texture::unbind()
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
