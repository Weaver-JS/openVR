#pragma once
#include <glm\glm.hpp>
#include <glm\gtx\rotate_vector.hpp>
#include <glm\gtx\transform.hpp>
#include "Shader.h"
#include "Constants.h"
class Camera
{
private:
	glm::mat4 projectMatrix;
	glm::mat4 viewMatrix;

	glm::mat4 translation;
	glm::mat4 rotation;

	Shader* shader;

	int vpUniform;

	void UpdateViewMatrix();
public:
	Camera(Shader* shader);
	Camera(Shader* shader,float w, float h);
	void setShader(Shader* shader);

	void setTranslation(glm::vec3 vector);
	void setRotation(glm::vec3 rotationVector, float rotationAngle);
	void Translate(glm::vec3 translationVector);
	void Rotate(glm::vec3 rotationVector, float rotationAngle);
	void Update();
	
	~Camera();
};

