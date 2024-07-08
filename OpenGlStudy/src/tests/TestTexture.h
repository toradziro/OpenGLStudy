#pragma once

#include "Test.h"

namespace test
{

class TestTexture : public ITest
{
public:
	TestTexture();
	~TestTexture();

	void onUpdate(float dt) override;
	void onRender() override;
	void onImGuiRender() override;

private:
	std::string m_texturePath;
};

}