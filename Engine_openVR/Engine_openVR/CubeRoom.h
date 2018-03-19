#pragma once
#include "Drawable.h"
#include "Shader.h"

class CubeRoom : public Drawable
{
public:
	CubeRoom(Shader* shader);
	~CubeRoom();
private:
	GLint positionAtt, colorAtt;
};

