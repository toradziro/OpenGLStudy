#include "TestBatchRenderingWithTextures.h"
#include "Renderer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

namespace test
{

TestBatchRenderingWithTextures::TestBatchRenderingWithTextures()
	: m_texturePath("res/textures/bomb.png"),
	m_translationA(100.0f, 100.0f, 0.0f)
{
	//icon_path.png
	//-- square
	float positions[] =
	{
		-50.0f, -50.0f, 0.9f, 0.1f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,	//-- vertex index 0
		50.0f, -50.0f, 0.9f, 0.1f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,	//-- vertex index 1
		50.0f, 50.0f, 0.9f, 0.1f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,		//-- vertex index 2
		-50.0f, 50.0f, 0.9f, 0.1f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,	//-- vertex index 3

		200.0f, 200.0f, 0.0f, 0.2f, 0.8f, 1.0f, 0.0f, 0.0f, 1.0f,	//-- vertex index 4
		300.0f, 200.0f, 0.0f, 0.2f, 0.8f, 1.0f, 1.0f, 0.0f, 1.0f,	//-- vertex index 5
		300.0f, 300.0f, 0.0f, 0.2f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f,	//-- vertex index 6
		200.0f, 300.0f, 0.0f, 0.2f, 0.8f, 1.0f, 0.0f, 1.0f, 1.0f,	//-- vertex index 7
	};

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
	m_vertexBuffer = std::make_unique<VertexBuffer>(positions, uint32_t(9 * 8 * sizeof(float)));

	m_layout = std::make_unique<VertexBufferLayout>();
	m_layout->push<float>(2);
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

TestBatchRenderingWithTextures::~TestBatchRenderingWithTextures()
{
}

void TestBatchRenderingWithTextures::onUpdate(float dt)
{
}

void TestBatchRenderingWithTextures::onRender()
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

void TestBatchRenderingWithTextures::onImGuiRender()
{
	ImGui::SliderFloat3("Translation A", &m_translationA.x, 0.0f, 1600.0f);
}

} // test