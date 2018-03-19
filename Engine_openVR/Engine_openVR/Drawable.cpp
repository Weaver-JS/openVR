#include "Drawable.h"



void Drawable::loadVertices(float data[], size_t size)
{
	this->vertices.insert(this->vertices.end(), data, data + size);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->vertices.size(), this->vertices.data(), GL_STATIC_DRAW);
}

void Drawable::loadVertices(std::vector<float> data)
{
	this->vertices.insert(this->vertices.end(), data.begin(), data.end());

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->vertices.size(), this->vertices.data(), GL_STATIC_DRAW);
}

void Drawable::loadElements(GLuint data[], size_t size)
{
	this->elements.insert(this->elements.end(), data, data + size);

	glGenBuffers(1, &this->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * this->elements.size(), this->elements.data(), GL_STATIC_DRAW);
}

void Drawable::loadElements(std::vector<GLuint> data)
{
	this->elements.insert(this->elements.end(), data.begin(), data.end());

	glGenBuffers(1, &this->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * this->elements.size(), this->elements.data(), GL_STATIC_DRAW);
}

void Drawable::bindVAO()
{
	glBindVertexArray(VAO);
}

void Drawable::unbindVAO()
{
	glBindVertexArray(0);
}

Drawable::Drawable()
{
	VAO = 0;
	VBO = 0;
	EBO = 0;
	glGenVertexArrays(1, &this->VAO);
}

bool Drawable::Draw()
{
	if (!VAO || !VBO || !EBO)
	{
		return false;
	}
	bindVAO();
	glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_INT, 0);
	unbindVAO();
	return true;
}


Drawable::~Drawable()
{
	if (EBO)
	{
		glDeleteBuffers(1, &EBO);
	}
	if (VBO)
	{
		glDeleteBuffers(1, &VBO);
	}
	if (VAO)
	{
		glDeleteVertexArrays(1, &VAO);
	}

}
