#include "TestState2.h"



TestState2::TestState2()
{
}


TestState2::~TestState2()
{
}

bool TestState2::init()
{
	glClearColor(1, 0, 0, 1);
	return true;
}

bool TestState2::update()
{
	glClear(GL_COLOR_BUFFER_BIT);// |GL_DEPTH_CLEAR_VALUE);
	const uint8_t* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_RIGHT])
	{
		std::cout << "Key Down" << std::endl;
	}
	return true;
}

bool TestState2::destroy()
{
	std::cout << "Deleted" << std::endl;
	delete this;
	return true;
}
