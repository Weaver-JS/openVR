#include "Camera.h"


Camera::Camera(Shader* shader)
{
	this->shader = shader;
	vpUniform = shader->getUniformLocation("mvpMatrix");
	projectMatrix = glm::perspective(glm::radians(45.0f), WINDOW_WIDTH / WINDOW_HEIGHT, 1.0f, 10000000.0f);//FOV, aspectRatio, zNear, zFar

}

Camera::Camera(Shader * shader, float w, float h)
{
	this->shader = shader;
	vpUniform = shader->getUniformLocation("mvpMatrix");
	projectMatrix = glm::perspective(glm::radians(45.0f), w / h, 0.01f, 30000.0f);//FOV, aspectRatio, zNear, zFar
}

void Camera::UpdateViewMatrix()
{
	viewMatrix = rotation * translation;
}


void Camera::setShader(Shader * shader)
{
	this->shader;
	vpUniform = shader->getUniformLocation("mvpMatrix");

}

void Camera::setTranslation(glm::vec3 vector)
{
	translation = glm::translate(glm::mat4(1.0f), -vector);
	UpdateViewMatrix();
}

void Camera::Translate(glm::vec3 vector)
{
	translation = glm::translate(translation, -vector);
	UpdateViewMatrix();
}

void Camera::setRotation(glm::vec3 rotationVector, float rotationAngle)
{
	this->rotation = glm::rotate(glm::mat4(1.0f),glm::radians(rotationAngle), -rotationVector);
	UpdateViewMatrix();
}

void Camera::Rotate(glm::vec3 rotationVector, float rotationAngle)
{
	rotation = glm::rotate(rotation, glm::radians(rotationAngle), -rotationVector);
	UpdateViewMatrix(); 
}

void Camera::Update()
{
	shader->setUniformMatrix(vpUniform, projectMatrix * viewMatrix );
}

Camera::~Camera()
{
}
