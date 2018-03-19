#include "TestState.h"



TestState::TestState()
{
}


TestState::~TestState()
{

	delete shader;
	delete testShape;
	delete room;
	delete camera;
}

bool TestState::init()
{
	glClearColor(0, 0, 0, 1);
	shader = new Shader("test_shader.vert", "test_shader.frag");
	shader->use_Shader();
	testShape = new TestShape(shader);
	room = new CubeRoom(shader);
	camera = new Camera(shader, WINDOW_WIDTH, WINDOW_HEIGHT);
	SDL_WarpMouseGlobal(500, 500);
	SDL_ShowCursor(0);
	camera->setTranslation(glm::vec3(0, 0, 500.0));
	//camRotation = glm::vec3(0.0f);

	camVelocity = glm::vec3(0.1f, 0.1f, 1.0f);
	return true;
}
const float MOUSE_SPEED = 0.05f;

bool TestState::update()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	room->Draw();
	this->testShape->Draw();
	

	const uint8_t* state = SDL_GetKeyboardState(NULL);
	camera->Update();
	if (state[SDL_SCANCODE_A])
	{
		camera->Translate(glm::vec3(0.01f, 0.0f, 0.0));
			/*glm::vec3(camVelocity.x * glm::cos(glm::radians(camRotation.y)),
				0,
				camVelocity.z * glm::sin(glm::radians(camRotation.y))));*/
	}
	if (state[SDL_SCANCODE_D])
	{
		camera->Translate(
			glm::vec3(camVelocity.x * glm::cos(glm::radians(camRotation.y)),
				0,
				camVelocity.z * glm::sin(glm::radians(camRotation.y))));
	}

	/*
	cos(0) = 1
	cos(90) = 0
	cos (180) -1
	cos(270) = 0

	sin(0) = 0
	sin(90) = 1
	sin(180) 0
	sin(270) = -1
	*/
	if (state[SDL_SCANCODE_W])
	{
		camera->Translate(
			glm::vec3(-camVelocity.x * glm::sin(glm::radians(camRotation.y)),
			0,
			-camVelocity.z * glm::cos(glm::radians(camRotation.y))));
	}
	if (state[SDL_SCANCODE_S])
	{
		camera->Translate(glm::vec3(0.0f, 0.0f, camVelocity.z));
			/*glm::vec3(camVelocity.x * glm::sin(glm::radians(camRotation.y)),
				0,
				camVelocity.z * glm::cos(glm::radians(camRotation.y))));*/
	}

	if (state[SDL_SCANCODE_LEFT])
	{
		camRotation.y += -1;
	}
	if (state[SDL_SCANCODE_RIGHT])
	{
		camRotation.y += 1;
	}

	if (state[SDL_SCANCODE_UP])
	{
		camRotation.x = -1;
	}
	if (state[SDL_SCANCODE_DOWN])
	{
		camRotation.x += 1;
	}
	int xPos, yPos;
	SDL_GetGlobalMouseState(&xPos, &yPos);
	SDL_WarpMouseGlobal(500, 500);

	camRotation.y += MOUSE_SPEED * float(500 - xPos);
	camRotation.x += MOUSE_SPEED * float(500 - yPos);
	camera->setRotation(glm::vec3(1.0f), 0.0f);
	camera->Rotate(glm::vec3(1, 0, 0), camRotation.x);
	camera->Rotate(glm::vec3(0, 1, 0), camRotation.y);
	

	return true;
}

bool TestState::destroy()
{
	std::cout << "Deleted" << std::endl;
	delete this;
	return true;
}
