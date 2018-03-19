#include "Window.h"



Window::Window(const std::string & tittle, int width, int height)
{
	sdlWindow = SDL_CreateWindow(tittle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_FLAG);
	if (this->sdlWindow == nullptr)
	{
		throw window_exception(SDL_GetError());
	}
	else
	{
		initRenderer();
	}
}

Window::Window(const std::string & tittle, int x, int y, int width, int height)
{
	sdlWindow = SDL_CreateWindow(tittle.c_str(),x,y,width,height, SDL_WINDOW_FLAG);
	if (this->sdlWindow == nullptr)
	{
		throw window_exception(SDL_GetError());
	}
	else
	{

	}
}

void Window::Update()
{
	SDL_GL_SwapWindow(sdlWindow);
	
}


Window::~Window()
{
	SDL_GL_DeleteContext(sdlWindow);
	SDL_DestroyWindow(sdlWindow);
}

void Window::initRenderer()
{
	sdlGLContext = SDL_GL_CreateContext(sdlWindow);
	if (sdlGLContext == nullptr)
	{
		throw glContextError(SDL_GetError());
	}
	else
	{
		if (SDL_GL_SetSwapInterval(1) < 0)
		{
			throw glContextError(SDL_GetError());
		}
	}

}
