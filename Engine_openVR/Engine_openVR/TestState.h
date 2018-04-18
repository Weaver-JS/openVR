#pragma once
#include "GameState.h"
#include "TestShape.h"
#include "CubeRoom.h"
#include "Shader.h"
#include "Camera.h"
#include "ObjLoader.h"

class TestState : public GameState 
{
private:
	Shader* shader;
	TestShape* testShape;
	Camera* camera;
	CubeRoom* room;
	ObjLoader* objTest;

	std::vector<Drawable*> drawables;
	glm::vec3 camRotation;
	glm::vec3 camVelocity;
	GLuint hasTextureUniform;
	GLuint instanceDataBuffer;
public:
	TestState();
	~TestState();

	void bindInstanceDataBuffer(InstanceData instanceData);
	void generateInstanceDataBuffer();
	bool init();
	bool update();
	bool destroy();
};

