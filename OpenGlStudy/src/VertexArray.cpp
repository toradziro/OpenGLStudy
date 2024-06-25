#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Renderer.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_rendererId));
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_rendererId));
}

void VertexArray::bind() const
{
	GLCall(glBindVertexArray(m_rendererId));
}

void VertexArray::unbind() const
{
	GLCall(glBindVertexArray(0));
}

void VertexArray::addBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout& layout) const
{
	bind();
	vertexBuffer.bind();
	const auto& elements = layout.elements();
	uint64_t offset = 0;
	for (size_t i = 0; i < elements.size(); ++i)
	{
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(GLuint(i)));
		GLCall(glVertexAttribPointer(GLuint(i), element.m_count, element.m_type, element.m_normalized,
			layout.stride(), (const void*)offset));
		offset += element.m_count * VertexBufferElement::elementSize(element.m_type);
	}
}
