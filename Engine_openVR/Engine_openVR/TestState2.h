#pragma once
#include "GameState.h"
#include "WindowManager.h"
class TestState2 : public GameState
{
private:
		
public:
	TestState2();
	~TestState2();
	bool init();
	bool update();
	bool destroy();
};

