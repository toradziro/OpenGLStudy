#pragma once

#include <vector>
#include <GL/glew.h>
#include "Renderer.h"

struct VertexBufferElement
{
	uint32_t	m_type;
	uint32_t	m_count;
	uint32_t	m_normalized;

	static uint32_t elementSize(uint32_t type)
	{
		switch (type)
		{
		case GL_FLOAT:
			return sizeof(GLfloat);
			break;
		case GL_UNSIGNED_INT:
			return sizeof(GLuint);
			break;
		case GL_UNSIGNED_BYTE:
			return sizeof(GLbyte);
			break;
		default:
			ASSERT(false);
			break;
		}
		return 0;
	}
};

class VertexBufferLayout
{
public:
	using VertexLayoutContainer = std::vector<VertexBufferElement>;

	VertexBufferLayout() : m_stride(0) {}
	~VertexBufferLayout() {}

	template<typename T>
	void push(uint32_t count)
	{
		ASSERT(false);
	}

	template<>
	void push<float>(uint32_t count)
	{
		m_elememts.push_back({ GL_FLOAT, count, GL_FALSE });
		m_stride += VertexBufferElement::elementSize(GL_FLOAT) * count;
	}

	template<>
	void push<uint32_t>(uint32_t count)
	{
		m_elememts.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_stride += VertexBufferElement::elementSize(GL_UNSIGNED_INT) * count;
	}

	template<>
	void push<uint8_t>(uint32_t count)
	{
		m_elememts.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_stride += VertexBufferElement::elementSize(GL_UNSIGNED_BYTE) * count;
	}

	inline uint32_t stride() const { return m_stride; }
	inline const VertexLayoutContainer& elements() const { return m_elememts; }

private:
	VertexLayoutContainer	m_elememts;
	uint32_t				m_stride;
};