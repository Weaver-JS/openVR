#pragma once
#include "GameState.h"
#include "TestShape.h"
#include "CubeRoom.h"
#include "Shader.h"
#include "Camera.h"

class TestState : public GameState 
{
private:
	Shader* shader;
	TestShape* testShape;
	Camera* camera;
	CubeRoom* room;
	glm::vec3 camRotation;
	glm::vec3 camVelocity;
public:
	TestState();
	~TestState();

	bool init();
	bool update();
	bool destroy();
};

