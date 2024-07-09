#pragma once

#include "Test.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <memory>

namespace test
{

class TestBatchRendering : public ITest
{
public:
	TestBatchRendering();
	~TestBatchRendering();

	void onUpdate(float dt) override;
	void onRender() override;
	void onImGuiRender() override;

private:
	std::unique_ptr<VertexBuffer>		m_vertexBuffer;
	std::unique_ptr<IndexBuffer>		m_indexBuffer;
	std::unique_ptr<VertexArray>		m_vertexArray;
	std::unique_ptr<VertexBufferLayout>	m_layout;
	std::unique_ptr<Shader>				m_shader;
	std::unique_ptr<Texture>			m_texture;

	std::string	m_texturePath;

	glm::vec3	m_translationA;
};

}