#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <glm\gtc\type_ptr.hpp>
#include <glm\matrix.hpp>
#include <GL\glew.h>


class Shader
{
	
private:
	std::ifstream vertFile, fragFile;
	std::string vertexFilename, fragmentFileName;
	std::stringstream vertexSourceStream, fragmentSourceStream;
	std::string vertexSource, fragmentSource;
	GLuint vertex_id, fragment_id, shaderprogram_id;


	bool ready;
	int Load();
	int Compile();
	int Link();
public:
	Shader(std::string vertexFilename, std::string fragmentFileName);
	bool use_Shader();
	bool set_mvp_matrix(const glm::mat4 mvp);
	GLint getAttribute(std::string name);
	GLint getUniformLocation(std::string name);
	void setUniformMatrix(GLint location, glm::mat4 matrix);
	~Shader();
};

