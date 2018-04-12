#include "TestShape.h"

GLfloat vData[] = {
	-0.5, 0.5f,50.0f,	1.0f, 0.0f,0.0f,		0.0f, 0.0f,
	 0.5f, 0.5f,50.0f,	 0.0f, 1.0f,1.0f,		1.0f,0.0f,
	 0.5f, -0.5f,50.0f,		0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
	 -0.5f, -0.5f,50.0f,	 1.0f, 1.0f,0.0f,	0.0f,1.0f
};

GLuint eData[] = {
	0, 1, 2,
	2, 3, 0 
};

TestShape::TestShape(Shader* shader) : Drawable()
{
	bindVAO();
	loadVertices(vData, 32);
	loadElements(eData, 6);
	positionAtt = shader->getAttribute("in_position");
	colorAtt = shader->getAttribute("in_color");
	uvAtt = shader->getAttribute("uv");

	glEnableVertexAttribArray(positionAtt);
	glVertexAttribPointer(positionAtt, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),0);
	glEnableVertexAttribArray(colorAtt);
	glVertexAttribPointer(colorAtt, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(uvAtt);
	glVertexAttribPointer(uvAtt, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	loadTexture("cucaracha.bmp");

}


TestShape::~TestShape()
{
}
