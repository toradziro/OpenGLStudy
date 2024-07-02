#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <windows.h>

#include <iostream>
#include <memory>
#include <limits>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"

#include "tests/TestClearColor.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#if defined(_MSC_VER)
float GetScreenScaleFactor()
{
	HDC screen = GetDC(0);
	int dpi = GetDeviceCaps(screen, LOGPIXELSX);
	ReleaseDC(0, screen);

	return dpi / 96.0f;
}
#endif

int main(int argc, char** argv)
{
	GLFWwindow* window;
	
	//-- Initialize the library
	if (!glfwInit())
	{
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//-- Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(1600, 800, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	
	//-- Make the window's context current
	glfwMakeContextCurrent(window);

	//-- Setting our framerate
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
	{
		std::cerr << __FUNCTION__ << " " << __LINE__ << std::endl;
		return -1;
	}
	
	std::cout << "GLFW version: " << glfwGetVersionString() << std::endl;
	std::cout << "GLEW version: " << glGetString(GL_VERSION) << std::endl;

	ImGui::CreateContext();

	//-- Set up sizes
#if defined(_MSC_VER)	
	float scaleFactor = GetScreenScaleFactor();
	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = scaleFactor;
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(scaleFactor);
#endif

	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

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

	//-- We need additional scope to let all resources go before we call glfwTerminate();
	{
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

		Shader shader = Shader("res/shaders/Basic.shader");
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

		test::TestClearColor testClearColor;

		//-- Loop until the user closes the window
		while (!glfwWindowShouldClose(window))
		{
			//-- Poll for and process events
			renderer.clear();

			testClearColor.onUpdate(0.0f);
			testClearColor.onRender();

			glfwPollEvents();
			ImGui_ImplGlfwGL3_NewFrame();

			testClearColor.onImGuiRender();

			{
				ImGui::SliderFloat3("Translation A", &translationA.x, 0.0f, 1600.0f);
				ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f, 1600.0f);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}


			{
				glm::mat4 modelA = glm::translate(glm::mat4(1.0f), translationA);
				glm::mat4 mvp = proj * view * modelA;

				shader.bind();
				shader.setUniformMat4f("u_ModuleViewProjection", mvp);
				renderer.draw(vertexArray, indexBuffer, shader);
			}

			{
				glm::mat4 modelB = glm::translate(glm::mat4(1.0f), translationB);
				glm::mat4 mvp = proj * view * modelB;

				shader.bind();
				shader.setUniformMat4f("u_ModuleViewProjection", mvp);
				renderer.draw(vertexArray, indexBuffer, shader);
			}

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
			//-- Swap front and back buffers
			glfwSwapBuffers(window);
			renderer.clear();
		}
	}

	//-- Cleanup ImGui & glfw
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();

	return 0;
}