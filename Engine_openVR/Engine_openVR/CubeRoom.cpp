#include "CubeRoom.h"

GLfloat roomVertices[] = {				/*Color*/
	-10.0f, 0, 0.5f,				1.0f, 0.0f, 0.0f,
	10.0f, 0,0.5f,					0.0f, 1.0f, 0.0f,
	10.0f, 0,-0.5f,					0.0f, 0.0f, 1.0f,
	-10.0f,0,-0.5f,					1.0f, 1.0f, 1.0f

	- 10.0f, -10.0f, 0.5f,				1.0f, 0.0f, 0.0f,
	10.0f, -10.0f,	0.5f,				0.0f, 1.0f, 0.0f,
	10.0f, -10.0f, 0.5f,				0.0f, 1.0f, 1.0f,
	-10.0f, -10.0f, 0.5f,				1.0f, 1.0f, 1.0f
};

GLuint roomElements[] = {
	//Front
	0, 1, 2,
	2, 3, 0,


//Back
	4, 5, 6,
	6, 7, 4,
	//Left
	4, 0, 3,
	3, 7, 4,

	//Right
	1, 5, 6,
	6, 2, 1,

	//Ceilling
	4, 5, 1,
	1, 0, 4,

	7, 6, 2,
	2, 3, 7

	
};

CubeRoom::CubeRoom(Shader* shader)
{
	bindVAO();
	loadVertices(roomVertices, 48);
	loadElements(roomElements, 36);
	positionAtt = shader->getAttribute("in_position");
	colorAtt = shader->getAttribute("in_color");

	glEnableVertexAttribArray(positionAtt);
	glVertexAttribPointer(positionAtt, 3, GL_FLOAT, GL_FALSE, 6* sizeof(float), 0);
	glEnableVertexAttribArray(colorAtt);
	glVertexAttribPointer(colorAtt, 3, GL_FLOAT, GL_FALSE, 6* sizeof(float), (void*)(3 * sizeof(float)));
}


CubeRoom::~CubeRoom()
{
}
