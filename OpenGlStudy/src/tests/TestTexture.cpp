#include "TestTexture.h"
#include "Renderer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

namespace test
{

TestTexture::TestTexture()
	: m_texturePath("res/textures/bomb.png"),
	m_translationA(100.0f, 100.0f, 0.0f),
	m_translationB(300.0f, 100.0f, 0.0f)
{
	//-- square
	float positions[] =
	{
		-50.0f, -50.0f, 0.0f, 0.0f,	//-- vertex index 1
		50.0f, -50.0f, 1.0f, 0.0f,	//-- vertex index 2
		50.0f, 50.0f, 1.0f, 1.0f,	//-- vertex index 3
		-50.0f, 50.0f, 0.0f, 1.0f,	//-- vertex index 4
	};

	uint32_t indexBufferData[] =
	{
		0, 1, 2,	//-- indices that describe first triangle for square
		2, 3, 0		//-- and second one
	};

	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	GLCall(glEnable(GL_BLEND));

	m_vertexArray = std::make_unique<VertexArray>();
	m_vertexBuffer = std::make_unique<VertexBuffer>(positions, uint32_t(4 * 4 * sizeof(float)));

	m_layout = std::make_unique<VertexBufferLayout>();
	m_layout->push<float>(2);
	m_layout->push<float>(2);
	m_vertexArray->addBuffer(*m_vertexBuffer, *m_layout);

	m_indexBuffer = std::make_unique<IndexBuffer>(indexBufferData, 2 * 3);

	m_shader = std::make_unique<Shader>("res/shaders/Basic.shader");
	m_shader->bind();

	m_texture = std::make_unique<Texture>("res/textures/bomb.png");
	m_texture->bind();
	//-- Must match with slot we bind texture to
	m_shader->setUniform1i("u_Texture", 0);

	m_shader->unbind();
}

TestTexture::~TestTexture()
{
}

void TestTexture::onUpdate(float dt)
{
}

void TestTexture::onRender()
{
	Renderer renderer;
	glm::mat4 proj = glm::ortho(0.0f, 1600.0f, 0.0f, 800.0f, -1.0f, 1.0f);
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	{
		glm::mat4 modelA = glm::translate(glm::mat4(1.0f), m_translationA);
		glm::mat4 mvp = proj * view * modelA;

		m_shader->bind();
		m_shader->setUniformMat4f("u_ModuleViewProjection", mvp);
		renderer.draw(*m_vertexArray, *m_indexBuffer, *m_shader);
	}

	{
		glm::mat4 modelB = glm::translate(glm::mat4(1.0f), m_translationB);
		glm::mat4 mvp = proj * view * modelB;

		m_shader->bind();
		m_shader->setUniformMat4f("u_ModuleViewProjection", mvp);
		renderer.draw(*m_vertexArray, *m_indexBuffer, *m_shader);
	}
}

void TestTexture::onImGuiRender()
{
	ImGui::SliderFloat3("Translation A", &m_translationA.x, 0.0f, 1600.0f);
	ImGui::SliderFloat3("Translation B", &m_translationB.x, 0.0f, 1600.0f);
}

} // test