#pragma once

#include "Test.h"

namespace test
{

class TestClearColor : public ITest
{
public:
	TestClearColor();
	~TestClearColor();

	void onUpdate(float dt) override;
	void onRender() override;
	void onImGuiRender() override;

private:
	float m_clearColor[4];
};

}