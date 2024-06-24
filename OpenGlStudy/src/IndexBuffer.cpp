#include "IndexBuffer.h"

#include "VertexBuffer.h"

#include "Renderer.h"

IndexBuffer::IndexBuffer(const uint32_t* data, uint32_t count) :
	m_indexesCount(count)
{
	ASSERT(sizeof(uint32_t) == sizeof(GLuint));

	GLCall(glGenBuffers(1, &m_rendererID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
	GLCall(glDeleteBuffers(1, &m_rendererID));
}

void IndexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID));
}

void IndexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
