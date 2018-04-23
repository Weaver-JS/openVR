#include "WindowManager.h"
#include <SDL2\SDL.h>
#include <OpenVR\openvr.h>
WindowManager* WindowManager::instance;

WindowManager*  WindowManager::getInstance()
{
	if (WindowManager::instance == nullptr)
	{
		WindowManager::instance = new WindowManager();
		WindowManager::instance->setState<TestState>();
	}
	return WindowManager::instance;
}

WindowManager::WindowManager()
{

	active = true;
	try 
	{
	
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
	
			throw window_exception(SDL_GetError());
		}
		else
		{
			this->window = new Window("OpenVR Engine 0.0.1", WINDOW_WIDTH, WINDOW_HEIGHT);
			glEnable(GL_DEPTH_TEST);
		}
	}
	catch (windowManager_exception & exception)
	{
		std::cout << "Window_Manager Error:  " << exception.what() << std::endl;
		#ifdef _DEBUG
				std::cout << "Line number: 32 " << "WindowManager.cpp"<<std::endl;
		#endif
		
	}

	catch (window_exception & exception)
	{
		std::cout << "Window Error:  " << exception.what() << std::endl;
		#ifdef _DEBUG
				std::cout << "Line number: 7 " << "Window.cpp" << std::endl;
		#endif
	}

	catch (render_exception & exception)
	{
		std::cout << "Renderer Error:  " << exception.what() << std::endl;
		#ifdef _DEBUG
				std::cout << "Line number: initRenderer() " << "Window.cpp" << std::endl;
		#endif
	}

	catch (glContextError & exc)
	{
		std::cout << "glContext Error:  " << exc.what() << std::endl;
		#ifdef _DEBUG
				std::cout << "Func: initRenderer() || glCOntext  " << "Window.cpp" << std::endl;
		#endif
	}
}

bool WindowManager::run()
{
	if (active)
	{
		
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				stop();
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_q:
					std::cout << "changeState TestState2" << std::endl;
					WindowManager::instance->setState<TestState2>();
					break;
				case SDLK_e:
					std::cout << "changeState TestState" << std::endl;
					WindowManager::instance->setState<TestState>();
					break;
				case SDLK_ESCAPE:
					std::cout << "Escape" << std::endl;
					stop();
					break;
				}
				break;
				
			}
		
			

		}
		if (this->state != nullptr && this->state->update())
		{
			window->Update();
		}

	}
	return active;
}

void WindowManager::stop()
{
	active = false;
}
WindowManager::~WindowManager()
{
	SDL_Quit();
}
