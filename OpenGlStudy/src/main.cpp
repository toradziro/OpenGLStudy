#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <windows.h>

#include <iostream>
#include <memory>
#include <limits>

#include "Renderer.h"

#include "tests/Test.h"
#include "tests/TestClearColor.h"
#include "tests/TestTexture.h"

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

	//-- We need additional scope to let all resources go before we call glfwTerminate();
	{
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		GLCall(glEnable(GL_BLEND));

		test::TestClearColor testClearColor;
		test::ITest* currentTest = nullptr;
		std::unique_ptr<test::TestMenu> testMenu = std::make_unique<test::TestMenu>(currentTest);
		currentTest = testMenu.get();

		testMenu->registerTest<test::TestClearColor>("Clear Color");
		testMenu->registerTest<test::TestTexture>("Test Texture");
		const float defaultClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		Renderer renderer;
		//-- Loop until the user closes the window
		while (!glfwWindowShouldClose(window))
		{
			//-- Poll for and process events
			renderer.setClearColor(defaultClearColor);
			renderer.clear();

			glfwPollEvents();
			ImGui_ImplGlfwGL3_NewFrame();

			if (currentTest != nullptr)
			{
				currentTest->onUpdate(0.0f);
				currentTest->onRender();

				ImGui::Begin("Test");
				if (currentTest != testMenu.get() && ImGui::Button("<- Back"))
				{
					delete currentTest;
					currentTest = testMenu.get();
				}
				currentTest->onImGuiRender();
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::End();
			}

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
			//-- Swap front and back buffers
			glfwSwapBuffers(window);
			renderer.clear();
		}

		if (currentTest != testMenu.get())
		{
			delete currentTest;
		}
	}

	//-- Cleanup ImGui & glfw
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();

	return 0;
}