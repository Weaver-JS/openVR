#pragma once
#include "Drawable.h"
#include "Shader.h"


class TestShape : public Drawable
{

private:
	GLint positionAtt, colorAtt;
public:
	TestShape(Shader* shader);
	~TestShape();
};

