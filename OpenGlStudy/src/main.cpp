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

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

float GetScreenScaleFactor()
{
	HDC screen = GetDC(0);
	int dpi = GetDeviceCaps(screen, LOGPIXELSX);
	ReleaseDC(0, screen);

	return dpi / 96.0f;
}

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
	window = glfwCreateWindow(2000, 1200, "Hello World", NULL, NULL);
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
	
	float scaleFactor = GetScreenScaleFactor();

	ImGui::CreateContext();

	//-- Set up sizes
	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = scaleFactor;
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(scaleFactor);


	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

	//-- square
	float positions[] =
	{
		200.0f, 200.0f, 0.0f, 0.0f,	//-- vertex index 1
		400.0f, 200.0f, 1.0f, 0.0f,	//-- vertex index 2
		400.0f, 400.0f, 1.0f, 1.0f,	//-- vertex index 3
		200.0f, 400.0f, 0.0f, 1.0f,	//-- vertex index 4
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

		glm::mat4 proj = glm::ortho(0.0f, 2000.0f, 0.0f, 1200.0f, -1.0f, 1.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, 0.0f, 0.0f));

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

		float r = 0.0f;
		float inc = 0.005f;

		glm::vec3 translation(0.0f, 0.0f, 0.0f);

		//-- Loop until the user closes the window
		while (!glfwWindowShouldClose(window))
		{
			//-- Poll for and process events
			renderer.clear();

			glfwPollEvents();
			ImGui_ImplGlfwGL3_NewFrame();

			//-- Render here
			shader.bind();

			renderer.draw(vertexArray, indexBuffer, shader);

			if (r > 1.0f)
			{
				inc = -inc;
			}
			else if (r < 0.0f)
			{
				inc = -inc;
			}

			r += inc;

			{
				ImGui::SliderFloat3("Translation", &translation.x, 0.0f, 2000.0f);

				glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
				glm::mat4 mvp = proj * view * model;
				shader.setUniformMat4f("u_ModuleViewProjection", mvp);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
			//-- Swap front and back buffers
			glfwSwapBuffers(window);
			
		}
	}

	//-- Cleanup ImGui & glfw
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();

	return 0;
}