#include "TestState.h"



TestState::TestState()
{
	instanceDataBuffer = 0;
}


TestState::~TestState()
{

	delete shader;
	delete testShape;
	delete room;
	delete camera;
}

void TestState::bindInstanceDataBuffer(InstanceData instanceData)
{
	glBindBuffer(GL_UNIFORM_BUFFER, instanceDataBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, // change the contents of the buffer
		0, // offset from the beginning of the buffer
		sizeof(InstanceData), // size of the changes
		(GLvoid*)&instanceData); // new content
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, instanceDataBuffer); // sets "instanceDataBuffer" to the slot "0"
}

void TestState::generateInstanceDataBuffer()
{
	glGenBuffers(1, &instanceDataBuffer); // creates a buffer. The buffer id is written to the variable "instanceDataBuffer"
	glBindBuffer(GL_UNIFORM_BUFFER, instanceDataBuffer); // sets "instanceDataBuffer" as the current buffer
	glBufferData(  // sets the buffer content
		GL_UNIFORM_BUFFER,		// type of buffer
		sizeof(InstanceData),	// size of the buffer content
		nullptr,				// content of the buffer
		GL_DYNAMIC_DRAW);		// usage of the buffer. DYNAMIC -> will change frequently. DRAW -> from CPU to GPU

								// we initialize the buffer without content so we can later call "glBufferSubData". Here we are only reserving the size.

	glBindBuffer(GL_UNIFORM_BUFFER, 0); // set no buffer as the current buffer
}

bool TestState::init()
{
	glClearColor(0, 0, 0.3f, 1);
	shader = new Shader("test_shader.vert", "test_shader.frag");
	shader->use_Shader();
	hasTextureUniform = shader->getUniformLocation("hasTexture");
	testShape = new TestShape(shader);
	objTest = new ObjLoader(shader, "ducky.obj");
	drawables.push_back(testShape);
	
	//room = new CubeRoom(shader);
	camera = new Camera(shader, WINDOW_WIDTH, WINDOW_HEIGHT);
	SDL_WarpMouseGlobal(500, 500);
	SDL_ShowCursor(0);
	camera->setTranslation(glm::vec3(0, 0, 500.0));
	testShape->setShader(*shader);
	//camRotation = glm::vec3(0.0f);
	vrManager.setSceneShader(*shader);
	vrManager.init();
	camVelocity = glm::vec3(0.1f, 0.1f, 1.0f);


	return true;
}
const float MOUSE_SPEED = 0.05f;

bool TestState::update()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//room->Draw();
	camera->Update();
	vrManager.update();
	vrManager.draw();
	glUniform1i(hasTextureUniform, true);
	for (auto drawable : drawables)
	{
		drawable->position += glm::vec3(0.1f, 1.0f, 0.0f);
		/*glm::mat4 translate = glm::translate(glm::mat4(), drawable->position);
		glm::mat4 scale = glm::scale(glm::mat4(), drawable->scale);
		glm::mat4 rotation = glm::rotate(glm::mat4(), (float)drawable->rotation.y, glm::vec3(0.0f, 0.0f, 1.0f));
		InstanceData instanceData = { translate * rotation * scale  , glm::vec4(0.5f,0.5f,0.0f,1.0f) };*/
	

		 // glm::value_ptr(drawable->position));
		
		drawable->Draw();
	}
	
	glUniform1i(hasTextureUniform, false);
	objTest->Draw();
	const uint8_t* state = SDL_GetKeyboardState(NULL);
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
	float PI = 3.14159265358979323846;
	float pitch = camRotation.x;
	float yaw = camRotation.y;
	float pitchRadian = pitch * (PI / 180);
	float yawRadian = yaw   * (PI / 180); // Y rotation
	float newPosX = camVelocity.x * sinf(yawRadian) * cosf(pitchRadian);
	float newPosY = camVelocity.y * -sinf(pitchRadian);
	float newPosZ = camVelocity.z * cosf(yawRadian) * cosf(pitchRadian);


	if (state[SDL_SCANCODE_W])
	{
		camera->Translate(
			glm::vec3(newPosX,
				-newPosY,
			-newPosZ));
	}
	if (state[SDL_SCANCODE_S])
	{
		camera->Translate(glm::vec3(0.0f, 0.0f, newPosZ));
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
	/*int xPos = 0, yPos= 0;
	SDL_GetGlobalMouseState(&xPos, &yPos);
	SDL_WarpMouseGlobal(500, 500);

	camRotation.y += MOUSE_SPEED * float(500 - xPos);
	camRotation.x += MOUSE_SPEED * float(500 - yPos);
	camera->setRotation(glm::vec3(1.0f), 0.0f);
	camera->Rotate(glm::vec3(1, 0, 0), camRotation.x);
	camera->Rotate(glm::vec3(0, 1, 0), camRotation.y);
	*/

	return true;
}

bool TestState::destroy()
{
	std::cout << "Deleted" << std::endl;
	delete this;
	return true;
}
