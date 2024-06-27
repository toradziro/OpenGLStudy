#pragma once

#include "Renderer.h"

class Texture
{
public:
	Texture(const std::string& path);
	~Texture();

	//-- We can have multiple slots for textures in OpenGL
	void bind(uint32_t slot = 0);
	void unbind();

	inline int width() const { return m_width; }
	inline int hieght() const { return m_hieght; }
	inline int bitsPerPixel() const { return m_bitsPerPixel; }

private:
	std::string	m_filePath;
	uint8_t*	m_localBuffer;
	uint32_t	m_rendererId;
	int			m_width;
	int			m_hieght;
	int			m_bitsPerPixel;
};