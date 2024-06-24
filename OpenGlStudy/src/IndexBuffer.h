#pragma once

#pragma once

#include <cstdint>

class IndexBuffer
{
public:
	IndexBuffer(const uint32_t* data, uint32_t count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	inline uint32_t count() const { return m_indexesCount; }

private:
	uint32_t m_rendererID;
	uint32_t m_indexesCount;
};