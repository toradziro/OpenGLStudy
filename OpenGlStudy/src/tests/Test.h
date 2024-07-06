#pragma once

#include <string>
#include <vector>
#include <utility>
#include <functional>

namespace test
{

class ITest
{
public:
	ITest() {}
	virtual ~ITest() {}

	virtual void onUpdate(float dt) {}
	virtual void onRender() {}
	virtual void onImGuiRender() {}
};

class TestMenu : public ITest
{
public:
	TestMenu(ITest*& currentTest);
	~TestMenu() {}

	void onImGuiRender() override;

	template<typename T>
	void registerTest(const std::string& name)
	{
		m_tests.push_back(std::make_pair(name, []()
			{
				return new T();
			}));
	}

private:
	using TestCollection = std::vector<std::pair<std::string, std::function<ITest*()>>>;

	TestCollection	m_tests;
	ITest*&			m_currentTest;
};

}