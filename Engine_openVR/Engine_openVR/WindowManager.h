#pragma once
#include "Window.h"
#include "TestState.h"
#include "TestState2.h"
#include "Constants.h"
#undef main

struct windowManager_exception : public std::runtime_error
{

public:
	windowManager_exception(const std::string &error) : runtime_error(error) {

	}
};


class WindowManager
{
	WindowManager();
	~WindowManager();
	static WindowManager*  instance;
private:
	Window* window;
	bool active;
	GameState* state = nullptr;
public:
	static WindowManager* getInstance();
	bool run();
	void stop();
	template <class T>
	void setState();

};

template<class T>
inline void WindowManager::setState()
{
	if (state != nullptr)
	{
		state->destroy();
	}
	state = new T();
	state->init();
}
