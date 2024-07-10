#pragma once

#include <cstdint>

class VertexBuffer
{
public:
	VertexBuffer(const void* data, uint32_t size);
	~VertexBuffer();

	void bind() const;
	void unbind() const;
	void loadData(const void* data, uint32_t size) const;

private:
	uint32_t m_rendererID;
};