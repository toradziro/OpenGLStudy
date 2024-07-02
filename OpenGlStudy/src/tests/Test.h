#pragma once

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

}