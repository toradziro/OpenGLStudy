#pragma once

#include <cstdint>

class VertexBuffer;
class VertexBufferLayout;

class VertexArray
{
public:

	VertexArray();
	~VertexArray();

	void bind() const;
	void unbind() const;
	void addBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout& layout) const;

private:
	uint32_t m_rendererId;
};