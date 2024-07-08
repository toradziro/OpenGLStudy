#include "TestTexture.h"
#include "Renderer.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

namespace test
{

TestTexture::TestTexture()
	: m_texturePath("res/textures/bomb.png")
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

	VertexArray vertexArray = VertexArray();
	VertexBuffer vertexBuffer = VertexBuffer(positions, uint32_t(4 * 4 * sizeof(float)));

	VertexBufferLayout layout;

	layout.push<float>(2);
	layout.push<float>(2);
	vertexArray.addBuffer(vertexBuffer, layout);

	IndexBuffer indexBuffer = IndexBuffer(indexBufferData, 2 * 3);

	glm::mat4 proj = glm::ortho(0.0f, 1600.0f, 0.0f, 800.0f, -1.0f, 1.0f);
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	Shader shader = Shader(m_texturePath);
	shader.bind();

	Texture texture("res/textures/bomb.png");
	texture.bind();
	//-- Must match with slot we bind texture to
	shader.setUniform1i("u_Texture", 0);

	vertexBuffer.unbind();
	indexBuffer.unbind();
	vertexArray.unbind();
	shader.unbind();

	Renderer renderer;

	glm::vec3 translationA(100.0f, 100.0f, 0.0f);
	glm::vec3 translationB(300.0f, 100.0f, 0.0f);
}

TestTexture::~TestTexture()
{

}

void TestTexture::onUpdate(float dt)
{
}

void TestTexture::onRender()
{
}

void TestTexture::onImGuiRender()
{
	ImGui::SliderFloat3("Translation A", &translationA.x, 0.0f, 1600.0f);
	ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f, 1600.0f);
}

} // test