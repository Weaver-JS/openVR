#pragma once
#include <vector>
#include <iterator>
#include <GL\glew.h>
#include <glm\matrix.hpp>
#include <SDL2\SDL.h>

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
	GLuint textureID;
	SDL_Surface * texture;

	std::vector<float> vertices;
	

protected:
	void loadVertices(float data[],size_t size);
	void loadVertices(std::vector<float> data);

	void loadElements(GLuint data[],size_t size);
	void loadElements(std::vector<GLuint> data);
	
	void loadTexture(std::string fileName);

	void bindVAO();
	void unbindVAO();

	std::vector<GLuint> elements;

};

