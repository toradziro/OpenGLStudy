#include "Test.h"

#include "imgui/imgui.h"

namespace test
{

test::TestMenu::TestMenu(ITest*& currentTest)
	: m_currentTest(currentTest)
{
}

void test::TestMenu::onImGuiRender()
{
	for (auto& test : m_tests)
	{
		if (ImGui::Button(test.first.c_str()))
		{
			m_currentTest = test.second();
		}
	}
}

}