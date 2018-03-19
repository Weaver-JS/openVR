#pragma once
#include <vector>
#include <iterator>
#include <GL\glew.h>
#include <glm\matrix.hpp>
struct InstanceData {
	glm::mat4 position;
	glm::vec4 color;
};
class Drawable
{
public:
	Drawable();
	bool Draw();
	~Drawable();

private:
	GLuint VAO, VBO, EBO;
	std::vector<GLuint> elements;
	std::vector<float> vertices;


protected:
	void loadVertices(float data[],size_t size);
	void loadVertices(std::vector<float> data);

	void loadElements(GLuint data[],size_t size);
	void loadElements(std::vector<GLuint> data);
	
	void bindVAO();
	void unbindVAO();

};

