#pragma once
#include <iostream>
#include <exception>
#include <SDL2\SDL.h>

 
#define SDL_WINDOW_FLAG SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
#define SDL_RENDERER_FLAG SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC


struct window_exception : public std::runtime_error
{

public:
	window_exception(const std::string &error) : runtime_error(error) {

	}
};

struct render_exception : public std::runtime_error
{

public:
	render_exception(const std::string &error) : runtime_error(error) {
		
	}
};

struct glContextError : public std::runtime_error
{

public:
	glContextError(const std::string &error) : runtime_error(error) {

	}
};

class Window
{
public:
	Window(const std::string& tittle, int width, int height);
	Window(const std::string & tittle, int x, int y , int width, int height);
	void Update();
	~Window();
private:
	void initRenderer();
	SDL_Window* sdlWindow;
	SDL_Renderer* renderer;
	SDL_GLContext sdlGLContext;
};

