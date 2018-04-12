#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <glm\glm.hpp>
#include <Windows.h>
#include <gl/glew.h>
#include "Drawable.h"
#include "Shader.h"


class ObjLoader : public Drawable
{
private:
	void HandleVertexData(std::string type, float x, float y, float z);
	void HandleFaceData(std::string triplet);
	unsigned int elementID;
	GLint positionAttribute,uvAttribute,colorAttribute;
	std::vector<float> vertices;
	std::vector<float> uvs;
	std::unordered_map<std::string, unsigned int> elementsCache;
	std::vector<unsigned int> element;
	std::vector<float> finalData;
	

public:
	ObjLoader(Shader* shader, std::string fileName);
	~ObjLoader();
};

