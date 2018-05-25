#include "Shader.h"



int Shader::Load()
{
	this->vertFile.open(vertexFilename, std::ios::in);
	if (!vertFile)
	{
		return -1;
	}
	this->vertexSourceStream << vertFile.rdbuf();

	this->fragFile.open(this->fragmentFileName, std::ios::in);
	if (!fragFile)
	{
		return -2;
	}
	fragmentSourceStream << fragFile.rdbuf();

	vertexSource = vertexSourceStream.str();
	fragmentSource = fragmentSourceStream.str();
	return 1;
}

int Shader::Compile()
{
	/*C++ to C conversion for OpenGL acceptance*/
	/*Vertex*/
	{
	const char* tempSource = vertexSource.c_str();
		vertex_id = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_id, 1, &tempSource, nullptr);
		glCompileShader(vertex_id);
		GLint compiled= 0;
		glGetShaderiv(vertex_id, GL_COMPILE_STATUS, &compiled);

		if (!compiled)
		{
			unsigned int maxLength;
			glGetShaderiv(vertex_id, GL_INFO_LOG_LENGTH, (GLint*)&maxLength);
			char* vertexCompileLog = (char*)malloc(maxLength);
			glGetShaderInfoLog(vertex_id, maxLength, (GLsizei*)&maxLength, vertexCompileLog);
			std::cout << "Vertex shader error" << std::endl << vertexCompileLog << std::endl;
			free(vertexCompileLog);
			return -1;
		}
	}
	/*Vertex*/

	/*Fragment*/
	{
		const char* tempSource = fragmentSource.c_str();
		fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_id, 1, &tempSource, 0);
		glCompileShader(fragment_id);
		GLint compiled;
		glGetShaderiv(fragment_id, GL_COMPILE_STATUS, &compiled);

		if (!compiled)
		{
			unsigned int maxLength;
			glGetShaderiv(fragment_id, GL_INFO_LOG_LENGTH, (GLint*)&maxLength);
			char* vertexCompileLog = (char*)malloc(maxLength);
			glGetShaderInfoLog(fragment_id, maxLength, (GLsizei*)&maxLength, vertexCompileLog);
			std::cout << "Fragment shader error" << std::endl << vertexCompileLog << std::endl;
			free(vertexCompileLog);
			return -2;
		}
	}
	/*Fragment*/
	return 1;
}

int Shader::Link()
{
	shaderprogram_id = glCreateProgram();
	glAttachShader(shaderprogram_id, vertex_id);
	glAttachShader(shaderprogram_id, fragment_id);
	glLinkProgram(shaderprogram_id);

	GLint linked;
	glGetProgramiv(shaderprogram_id, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		unsigned int maxLength;
		glGetProgramiv(shaderprogram_id, GL_INFO_LOG_LENGTH, (GLint*)&maxLength);
		char* programLinkLog = (char*)malloc(maxLength);
		glGetProgramInfoLog(shaderprogram_id, maxLength, (GLsizei*)&maxLength, programLinkLog);
		std::cout << "Program Link Error " << std::endl << programLinkLog << std::endl << std::endl;
		free(programLinkLog);
		return -1;
	}
	return 1;
}

Shader::Shader(std::string vertexFilename, std::string fragmentFileName)
{
	GLenum err = glewInit();
	ready = false;
	this->vertexFilename = vertexFilename;
	this->fragmentFileName = fragmentFileName;
	if (this->Load() < 1)
	{
		std::cout << "Error loading shader sources!, make sure the file is correctly typed" << std::endl;
		return;
	}

	if (this->Compile() < 1)
	{
		std::cout << "Error compiling shader sources!" << std::endl;
			return;
	}
	
	if (this->Link() < 1)
	{
		std::cout << "Error linking shader sources!" << std::endl;
		return;
	}
	this->ready = true;
}

bool Shader::use_Shader()
{
	if (ready)
	{
		glUseProgram(shaderprogram_id);
	}
	return ready;
}

GLint Shader::getAttribute(std::string name)
{
	return glGetAttribLocation(shaderprogram_id, name.c_str());
}

GLint Shader::getUniformLocation(std::string name)
{
	return glGetUniformLocation(shaderprogram_id, name.c_str());
}

bool Shader::set_mvp_matrix(const glm::mat4 mvp)
{
	int matrixID = glGetUniformLocation(shaderprogram_id, "mvp");
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(mvp));

	return true;
}
void Shader::setUniformMatrix(GLint location, glm::mat4 matrix)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}


Shader::~Shader()
{
}
