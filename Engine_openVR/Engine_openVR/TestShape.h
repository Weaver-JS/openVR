#pragma once
#include "Drawable.h"
#include "Shader.h"


class TestShape : public Drawable
{

private:
	GLint positionAtt, colorAtt, uvAtt;
public:
	TestShape(Shader* shader);
	~TestShape();
};

