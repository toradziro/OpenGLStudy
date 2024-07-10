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

class TestBatchRenderingDynamicGeometry : public ITest
{
public:
	TestBatchRenderingDynamicGeometry();
	~TestBatchRenderingDynamicGeometry();

	void onUpdate(float dt) override;
	void onRender() override;
	void onImGuiRender() override;

private:
	std::unique_ptr<VertexBuffer>			m_vertexBuffer;
	std::unique_ptr<IndexBuffer>			m_indexBuffer;
	std::unique_ptr<VertexArray>			m_vertexArray;
	std::unique_ptr<VertexBufferLayout>		m_layout;
	std::unique_ptr<Shader>					m_shader;
	std::vector<std::unique_ptr<Texture>>	m_textures;

	glm::vec3	m_translationA;
	float		m_quadPosition[2] = { -50.0f, -50.0f };
	float		m_secondQuadPosition[2] = { 200.0f, 200.0f };
};

}