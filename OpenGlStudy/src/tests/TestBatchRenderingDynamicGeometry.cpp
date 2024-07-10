#include "TestBatchRenderingDynamicGeometry.h"
#include "Renderer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include <array>

namespace
{

struct Vertex
{
	float m_position[3];
	float m_color[4];
	float m_textureCoord[2];
	float m_textureId;
};

std::array<Vertex, 4> generateQuad(float x, float y, float textureId)
{
	float size = 100.0f;

	Vertex v0 {
		.m_position{ x, y, 0.0f },
		.m_color{ 0.9f, 0.1f, 0.0f, 1.0f },
		.m_textureCoord{ 0.0f, 0.0f },
		.m_textureId{ textureId }
	};

	Vertex v1 {
		.m_position{ x + size, y, 0.0f },
		.m_color{ 0.9f, 0.1f, 0.0f, 1.0f },
		.m_textureCoord{ 1.0f, 0.0f },
		.m_textureId{ textureId }
	};

	Vertex v2 {
		.m_position{ x + size, y + size, 0.0f },
		.m_color{ 0.9f, 0.1f, 0.0f, 1.0f },
		.m_textureCoord{ 1.0f, 1.0f },
		.m_textureId{ textureId }
	};

	Vertex v3 {
		.m_position{ x, y + size, 0.0f },
		.m_color{ 0.9f, 0.1f, 0.0f, 1.0f },
		.m_textureCoord{ 0.0f, 1.0f },
		.m_textureId{ textureId }
	};

	return { v0, v1, v2, v3 };
}

}

namespace test
{

TestBatchRenderingDynamicGeometry::TestBatchRenderingDynamicGeometry()
	: m_translationA(100.0f, 100.0f, 0.0f)
{
	uint32_t indexBufferData[] =
	{
		0, 1, 2,	//-- indices that describe first triangle for square
		2, 3, 0,	//-- and second one

		4, 5, 6,
		6, 7, 4
	};

	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	GLCall(glEnable(GL_BLEND));

	m_vertexArray = std::make_unique<VertexArray>();
	m_vertexBuffer = std::make_unique<VertexBuffer>(nullptr, sizeof(Vertex) * 200);

	m_layout = std::make_unique<VertexBufferLayout>();
	m_layout->push<float>(3);
	m_layout->push<float>(4);
	m_layout->push<float>(2);
	m_layout->push<float>(1);
	m_vertexArray->addBuffer(*m_vertexBuffer, *m_layout);

	m_indexBuffer = std::make_unique<IndexBuffer>(indexBufferData, 4 * 3);

	m_shader = std::make_unique<Shader>("res/shaders/BatchTexture.shader");

	m_textures.push_back(std::make_unique<Texture>("res/textures/bomb.png"));
	m_textures.push_back(std::make_unique<Texture>("res/textures/icon_path.png"));

	m_shader->bind();

	for (size_t i = 0; i < m_textures.size(); ++i)
	{
		m_textures[i]->bind(i);
		//-- Must match with slot we bind texture to
		m_shader->setUniform1i("u_Textures", i);
	}

	m_shader->unbind();
}

TestBatchRenderingDynamicGeometry::~TestBatchRenderingDynamicGeometry()
{
}

void TestBatchRenderingDynamicGeometry::onUpdate(float dt)
{
	auto firstQuad = generateQuad(m_quadPosition[0], m_quadPosition[1], 0.0f);
	auto secondQuad = generateQuad(m_secondQuadPosition[0], m_secondQuadPosition[1], 1.0f);

	Vertex vertecies[8];
	memcpy(vertecies, firstQuad.data(), sizeof(Vertex) * firstQuad.size());
	memcpy(vertecies + firstQuad.size(), secondQuad.data(), sizeof(Vertex) * secondQuad.size());

	m_vertexBuffer->loadData(vertecies, sizeof(vertecies));
}

void TestBatchRenderingDynamicGeometry::onRender()
{
	Renderer renderer;
	glm::mat4 proj = glm::ortho(0.0f, 1600.0f, 0.0f, 800.0f, -1.0f, 1.0f);
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	glm::mat4 modelA = glm::translate(glm::mat4(1.0f), m_translationA);
	glm::mat4 mvp = proj * view * modelA;

	m_shader->bind();
	m_shader->setUniformMat4f("u_ModuleViewProjection", mvp);
	renderer.draw(*m_vertexArray, *m_indexBuffer, *m_shader);
}

void TestBatchRenderingDynamicGeometry::onImGuiRender()
{
	ImGui::SliderFloat3("Translation", &m_translationA.x, 0.0f, 1600.0f);
	ImGui::DragFloat2("First Quad", m_quadPosition, 0.5f);
	ImGui::DragFloat2("Second Quad", m_secondQuadPosition, 0.5f);
}

} // test