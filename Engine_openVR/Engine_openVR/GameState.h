#pragma once
#include <SDL2/SDL.h>
#if defined(_WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
#include <windows.h>
#endif
#include <GL/glew.h>
#include <GL/GL.h>
#include <iostream>

/*
 Clase abstracta que nos permite funcionar como interfaz entre clases
  
*/
class GameState
{
private:
	
public:
	GameState();
	~GameState();
	virtual bool init() = 0;
	virtual bool update() = 0;
	virtual bool destroy() = 0;
};

