#include "VR_Manager.h"



int VR_Manager::initOpenVR()
{
	 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, -3.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));
	if (vr::VR_IsHmdPresent())
	{
		std::cout << "HMD succesfully connected" << std::endl;
		if (vr::VR_IsRuntimeInstalled())
		{
			const char* runtimePath = vr::VR_RuntimePath();
			std::cout << "Runtime path: " << runtimePath << std::endl;
		}
		else
		{
			std::cout << "Runtime was not found " << std::endl;
			return -1;
		}
	}
	else
	{
		std::cout << "No HMD was found in the system, quitting app" << std::endl;
		return -1;
	}
	
	vr::HmdError hmdError;
	vrContext = vr::VR_Init(&hmdError, vr::EVRApplicationType::VRApplication_Scene); //init openVR
	vrContext == NULL ? std::cout << "Error initializing VRsystem. " << hmdError << std::endl : std::cout << "VRsystem initialized successfully" << std::endl;
	
	//Cargamos todos los modelos del HMD, en este caso al ser VIVE nos referimos a trackers, base stations...
	vrModels = (vr::IVRRenderModels*) vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &hmdError);
	if(!vrModels)
	{
		vr::VR_Shutdown();
		std::cout << "Couldn't load generic render models" << std::endl;
		return -1;
	}
	else
	{
		std::cout << "Render models loaded succesfully" << std::endl;
	}
	

	for (uint32_t actualDevice = vr::k_unTrackedDeviceIndex_Hmd; actualDevice < vr::k_unMaxTrackedDeviceCount; ++actualDevice)
	{
		if (vrContext->IsTrackedDeviceConnected(actualDevice))
		{
			vr::ETrackedDeviceClass trackedDeviceClass = vrContext->GetTrackedDeviceClass(actualDevice);
			std::cout << "Tracking device " << actualDevice << " is connected" << std::endl;

			std::cout << "Device type: " << trackedDeviceClass << std::endl;

			if (trackedDeviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_TrackingReference) base_stations_count++; //Trackeamos una de las base stations

			if (actualDevice == vr::k_unTrackedDeviceIndex_Hmd)
			{
				m_strDriver = trackedDeviceClass;//vr::GetTrackedDeviceString(vr_context, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String);;

				m_strDisplay = vr::Prop_SerialNumber_String;
			}
			else
			{
				if (!setup_render_model(actualDevice))
				{
					return -1;
				}
			}
		}
		else
			std::cout << "Tracking device " << actualDevice << "not Connected" << std::endl;
	}
		if (base_stations_count < 2)
		{
			std::cout << "Problem identifying base stations " << std::endl;
			return -1;
		}

		if (!(vrCompositor = vr::VRCompositor()))
		{
			std::cout << " Compositior initialization failed" << std::endl;
			return -1;
		}
		else
		{
			std::cout << "Compositor successfully initialized" << std::endl;
		}

		vrContext->GetRecommendedRenderTargetSize(&render_width, &render_height);
		std::cout << "Recommended render target size is" << render_width << " & " << render_height << std::endl;

		std::cout << "Setting up left eye frame buffer..." << std::endl;
		if (!setup_frame_buffer(render_width, render_height, l_frame_buffer, l_render_buffer_depth_stencil, l_tex_color_buffer)) return -1;
		std::cout << "Left eye frame buffer setup ready" << std::endl;

		std::cout << "Setting up right eye frame buffer..." << std::endl;
		if (!setup_frame_buffer(render_width, render_height, r_frame_buffer, r_render_buffer_depth_stencil, r_tex_color_buffer)) return -1;
		std::cout << "Right eye frame buffer ready" << std::endl;

		projectionMatrixLeft = getProjectionMatrix(vr::Hmd_Eye::Eye_Left);
		projectionMatrixRight = getProjectionMatrix(vr::Hmd_Eye::Eye_Right);
		viewMatrixLeft = getViewMatrix(vr::Hmd_Eye::Eye_Left);
		viewMatrixRight = getViewMatrix(vr::Hmd_Eye::Eye_Right);
	
	return 0;
}


void VR_Manager::processVREvent(const vr::VREvent_t & vrEvent)
{
	if(triggerModel != NULL)
	triggerModel->modelMatrix = nullptr;
	std::string str_td_class = GetTrackedDeviceClassString(vrContext->GetTrackedDeviceClass(vrEvent.trackedDeviceIndex));

	switch (vrEvent.eventType)
	{
	case vr::VREvent_TrackedDeviceActivated:
	{
		std::cout << "Device " << vrEvent.trackedDeviceIndex << " attached (" << str_td_class << "). Setting up render model" << std::endl;

		// Load render models for the tracking device (when it's powered on duriing application execution)
		setup_render_model(vrEvent.trackedDeviceIndex);
	}
	break;
	case vr::VREvent_TrackedDeviceDeactivated:
	{
		std::cout << "Device " << vrEvent.trackedDeviceIndex << " detached (" << str_td_class << "). Removing render model" << std::endl;

		RenderModel* render_model = render_models[vrEvent.trackedDeviceIndex];
		render_models[vrEvent.trackedDeviceIndex] = NULL;

		delete render_model;
	}
	break;
	case vr::VREvent_TrackedDeviceUpdated:
	{
		std::cout << "Device " << vrEvent.trackedDeviceIndex << " updated (" << str_td_class << ")" << std::endl;
	}
	break;
	case vr::VREvent_ButtonPress:
	{
		vr::VREvent_Controller_t controller_data = vrEvent.data.controller;
		std::cout << "Pressed button " << vrContext->GetButtonIdNameFromEnum((vr::EVRButtonId)controller_data.button) << " of device " << vrEvent.trackedDeviceIndex << " (" << str_td_class << ")" << std::endl;

		// Another way of accessing the state of the controller...
		vr::VRControllerState_t controller_state;
		vr::TrackedDevicePose_t td_pose;
		if (vrContext->GetControllerStateWithPose(vr::ETrackingUniverseOrigin::TrackingUniverseStanding, vrEvent.trackedDeviceIndex, &controller_state, sizeof(controller_state), &td_pose)) {
			if ((vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_Axis1) & controller_state.ulButtonPressed) != 0) {
				std::cout << "Trigger button pressed!" << std::endl;
				triggerModel->modelMatrix = &trackedDevicesPoses[4].mDeviceToAbsoluteTracking ;
				std::cout << "Pose information" << std::endl;
				std::cout << "  Tracking result: " << td_pose.eTrackingResult << std::endl;
				std::cout << "  Tracking velocity: (" << td_pose.vVelocity.v[0] << "," << td_pose.vVelocity.v[1] << "," << td_pose.vVelocity.v[2] << ")" << std::endl;
			}
			
		}
	}
	break;
	case vr::VREvent_ButtonUnpress:
	{
		vr::VREvent_Controller_t controller_data = vrEvent.data.controller;
		std::cout << "Unpressed button " << vrContext->GetButtonIdNameFromEnum((vr::EVRButtonId)controller_data.button) << " of device " << vrEvent.trackedDeviceIndex << " (" << str_td_class << ")" << std::endl;
	}
	break;
	case vr::VREvent_ButtonTouch:
	{
		vr::VREvent_Controller_t controller_data = vrEvent.data.controller;
		std::cout << "Touched button " << vrContext->GetButtonIdNameFromEnum((vr::EVRButtonId)controller_data.button) << " of device " << vrEvent.trackedDeviceIndex << " (" << str_td_class << ")" << std::endl;
	}
	break;
	case vr::VREvent_ButtonUntouch:
	{
		vr::VREvent_Controller_t controller_data = vrEvent.data.controller;
		std::cout << "Untouched button " << vrContext->GetButtonIdNameFromEnum((vr::EVRButtonId)controller_data.button) << " of device " << vrEvent.trackedDeviceIndex << " (" << str_td_class << ")" << std::endl;
	}
	break;
	}
}

bool VR_Manager::setup_render_model(vr::TrackedDeviceIndex_t tracked_device)
{
	if (tracked_device >= vr::k_unMaxTrackedDeviceCount)
	return false;

	std::string render_model_name = GetTrackedDeviceString(vrContext, tracked_device, vr::Prop_RenderModelName_String);
	render_models[tracked_device] = new RenderModel(render_model_name.c_str());

	std::cout << "Starting initialization of render model data for device " << tracked_device << std::endl;
	if (render_models[tracked_device]->init())
	{
		std::cout << "Render model for device " << tracked_device << " correctly initialized" << std::endl;
		return true;
	}
	std::cout << "Unable to generate render model data for device " << tracked_device << std::endl;
	return false;
}

bool VR_Manager::setup_scene_buffers()
{
	//// Create and bind a vertex array for storing both cube attributes (positions and colors)
	//glGenVertexArrays(1, &cube_texture_vao);
	//glBindVertexArray(cube_texture_vao);

	//// Create and bind a vertex buffer for cube vertex positions and copy the data from the cube vertices
	//glGenBuffers(1, &cube_texture_vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, cube_texture_vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(cube_tex), cube_tex, GL_STATIC_DRAW);

	//// Create a texture buffer
	//glGenTextures(1, &cube_texture);

	//// TEXTURE 1...

	//glBindTexture(GL_TEXTURE_2D, cube_texture);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//// Load texture image
	//int width, height;
	//unsigned char* image = SOIL_load_image(".\\.\\portal_cube.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	//glGenerateMipmap(GL_TEXTURE_2D);

	//SOIL_free_image_data(image);

	//// Create and bind a vertex array for storing square attributes (positions, colors and textures)
	//// In this square we are going to map the texture from the frame buffer containing the scene
	//glGenVertexArrays(1, &square_texture_vao);
	//glBindVertexArray(square_texture_vao);

	//// Create and bind a vertex buffer for square vertex positions and copy the data from the square vertices
	//glGenBuffers(1, &square_texture_vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, square_texture_vbo);
	//glBufferData(GL_ARRAY_BUFFER, vertices*floatsPerVertex * sizeof(GLfloat), square_tex, GL_STATIC_DRAW);

	//// Create and bind an element buffer for square vertex indexes and copy the data from the square vertices
	//glGenBuffers(1, &square_texture_ebo);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, square_texture_ebo);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	std::cout << "Initializing shaders..." << std::endl;

	/*if (scene_shader->use_Shader())
	{
		//scene_shader->use_program();

		glBindVertexArray(cube_texture);
		glBindBuffer(GL_ARRAY_BUFFER, cube_texture);

		// Specify the layout of the vertex data
		GLint posAttrib = glGetAttribLocation(scene_shader->get_shader(), "in_Position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, floatsPerVertex * sizeof(GLfloat), (GLvoid*)0);

		GLint colAttrib = glGetAttribLocation(scene_shader->get_shader(), "in_Color");
		glEnableVertexAttribArray(colAttrib);
		glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, floatsPerVertex * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

		GLint texAttrib = glGetAttribLocation(scene_shader->get_shader(), "in_Texcoord");
		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, floatsPerVertex * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cube_texture);
		glUniform1i(glGetUniformLocation(scene_shader->get_shader(), "tex"), 0);

		cout << "Scene shaders successfully initialized" << endl;
	}
	else
		return false;*/

	if (screen_shader->use_Shader())
	{
		//screen_shader->use_program();

		glBindVertexArray(square_texture_vao);
		glBindBuffer(GL_ARRAY_BUFFER, square_texture_vbo);

		// Specify the layout of the vertex data
		GLint posAttrib = screen_shader->getAttribute("in_Position");// glGetAttribLocation(screen_shader->get_shader(), "in_Position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, floatsPerVertex * sizeof(GLfloat), (GLvoid*)0);

		GLint colAttrib = screen_shader->getAttribute("in_Color");
		glEnableVertexAttribArray(colAttrib);
		glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, floatsPerVertex * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

		GLint texAttrib = screen_shader->getAttribute("in_Texcoord");
		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, floatsPerVertex * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));

		cout << "Screen shaders successfully initialized" << endl;
	}
	else
		return false;

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind EBO
	glBindVertexArray(0); // Unbind VAO

	return true;
}

bool VR_Manager::setup_frame_buffer(GLsizei width, GLsizei height, GLuint & frame_buffer, GLuint & render_buffer_depth_stencil, GLuint & tex_color_buffer)
{
	// Create and bind the frame buffer
	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

	// Create texture where we are going to bulk the contents of the frame buffer
	glGenTextures(1, &tex_color_buffer);
	glBindTexture(GL_TEXTURE_2D, tex_color_buffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Attach the image to the framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_color_buffer, 0); // The second parameter implies that you can have multiple color attachments. A fragment shader can output
																									  // different data to any of these by linking 'out' variables to attachments with the glBindFragDataLocation function

																									  // Create the render buffer to host the depth and stencil buffers
																									  // Although we could do this by creating another texture, it is more efficient to store these buffers in a Renderbuffer Object, because we're only interested in reading the color buffer in a shader
	glGenRenderbuffers(1, &render_buffer_depth_stencil);
	glBindRenderbuffer(GL_RENDERBUFFER, render_buffer_depth_stencil);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

	// Attach the render buffer to the framebuffer
	// NOTE: Even if you don't plan on reading from this depth_attachment, an off screen buffer that will be rendered to should have a depth attachment
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_buffer_depth_stencil);

	// Check whether the frame buffer is complete (at least one buffer attached, all attachmentes are complete, all attachments same number multisamples)
	(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) ? std::cout << "The frame buffer is complete!" << std::endl : std::cout << "The frame buffer is invalid, please re-check. Code: " << glCheckFramebufferStatus(l_frame_buffer) << std::endl;

	return true;
}

glm::mat4 VR_Manager::convertStemVRMatrixToGLMMatrix(const vr::HmdMatrix34_t & matPose)
{
	glm::mat4 pose_matrix = glm::mat4(matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0f,
		matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0f,
		matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0f,
		matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f); // As glm stores matrices in column major order and HmdMatrix34 are in row major order, ther first 3 elements of pose_matrix
																  // should be the x.x, y.x and z.x coords which corresponds to HmdMatrix34[0][0], HmdMatrix34[1][0], HmdMatrix34[2][0]
																  // The last row of the pose_matrix should be the position of the coordinate system, (0,0,0,1) in this case
																  // More info. abour matrix order in OpenVR and glm here: https://github.com/ValveSoftware/openvr/issues/193

	return pose_matrix;
}

glm::mat4 VR_Manager::getProjectionMatrix(const vr::Hmd_Eye eye)
{
	vr::HmdMatrix44_t steamvr_proj_matrix = vrContext->GetProjectionMatrix(eye, 0.1f, 15.f);
	return glm::mat4(steamvr_proj_matrix.m[0][0], steamvr_proj_matrix.m[1][0], steamvr_proj_matrix.m[2][0], steamvr_proj_matrix.m[3][0],
		steamvr_proj_matrix.m[0][1], steamvr_proj_matrix.m[1][1], steamvr_proj_matrix.m[2][1], steamvr_proj_matrix.m[3][1],
		steamvr_proj_matrix.m[0][2], steamvr_proj_matrix.m[1][2], steamvr_proj_matrix.m[2][2], steamvr_proj_matrix.m[3][2],
		steamvr_proj_matrix.m[0][3], steamvr_proj_matrix.m[1][3], steamvr_proj_matrix.m[2][3], steamvr_proj_matrix.m[3][3]);
}

glm::mat4 VR_Manager::getViewMatrix(const vr::Hmd_Eye eye)
{
	vr::HmdMatrix34_t steamvr_eye_view_matrix = vrContext->GetEyeToHeadTransform(eye);

	glm::mat4 view_matrix = glm::mat4(steamvr_eye_view_matrix.m[0][0], steamvr_eye_view_matrix.m[1][0], steamvr_eye_view_matrix.m[2][0], 0.0f,
		steamvr_eye_view_matrix.m[0][1], steamvr_eye_view_matrix.m[1][1], steamvr_eye_view_matrix.m[2][1], 0.0f,
		steamvr_eye_view_matrix.m[0][2], steamvr_eye_view_matrix.m[1][2], steamvr_eye_view_matrix.m[2][2], 0.0f,
		steamvr_eye_view_matrix.m[0][3], steamvr_eye_view_matrix.m[1][3], steamvr_eye_view_matrix.m[2][3], 1.0f);

	return glm::inverse(view_matrix);
}

void VR_Manager::setSceneShader(Shader & scene_shader)
{
	this->scene_shader = &scene_shader;
	quad = new TestShape(&scene_shader);
	quad->setShader(scene_shader);
}

std::string VR_Manager::GetTrackedDeviceClassString(vr::ETrackedDeviceClass deviceClass)
{
	std::string str_td_class = "Unknown class";
	switch (deviceClass)
	{
	case vr::ETrackedDeviceClass::TrackedDeviceClass_Invalid:
		str_td_class = "Invalid";
		break;
	case vr::ETrackedDeviceClass::TrackedDeviceClass_HMD:
		str_td_class = "HMD";
		break;
	case vr::ETrackedDeviceClass::TrackedDeviceClass_Controller:			// = 2, Tracked controllers
		str_td_class = "Controller";
		break;
	case vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker:		// = 3, Generic trackers, similar to controllers
		str_td_class = "Generic Tracker";
		break;
	case vr::ETrackedDeviceClass::TrackedDeviceClass_TrackingReference:	// = 4, Camera and base stations that serve as tracking reference points
		str_td_class = "Tracking Reference";
		break;
	case vr::ETrackedDeviceClass::TrackedDeviceClass_DisplayRedirect:	// = 5, Accessories that aren't necessarily tracked themselves, but may redirect video output from other tracked devices
		str_td_class = "Display Redirecd";
		break;
	}
	return str_td_class;
}

std::string VR_Manager::GetTrackedDeviceString(vr::IVRSystem * pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError * peError)
{
	uint32_t requiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, NULL, 0, peError);
	if (requiredBufferLen == 0)
		return "";

	char *pchBuffer = new char[requiredBufferLen];
	requiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, requiredBufferLen, peError);
	std::string sResult = pchBuffer;
	delete[] pchBuffer;
	return sResult;
}

VR_Manager::VR_Manager()
{

}


VR_Manager::~VR_Manager()
{
	exit();
	delete render_model_shader;
	delete screen_shader;
}

int VR_Manager::init()
{
	vrContext = NULL;
	vrModels = NULL;
	vrCompositor = NULL;
	base_stations_count = 0;
	time = (float)SDL_GetTicks();
	viewMatrixLeft = viewMatrixRight = glm::mat4(1.0f);
	projectionMatrixLeft = projectionMatrixRight = glm::mat4(1.0f);
	for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
	{
		trackedDevicePoseMatrix[nDevice] = glm::mat4(1.0f);
		render_models[nDevice] = NULL;
	}

	/*
	Shaders
	*/
	//scene_shader = new Shader(".\\.\\src\\scene_shader.vert", ".\\.\\src\\scene_shader.frag", "");
	screen_shader = new Shader("screen_shader.vert","screen_shader.frag");
	render_model_shader = new Shader("render_model_shader.vert", "render_model_shader.frag");

	m_strDriver = "No Driver";
	m_strDisplay = "";
	
	return initOpenVR();
	
}

void VR_Manager::update()
{
	if (vrContext != NULL)
	{
		// Process SteamVR events
		vr::VREvent_t vr_event;
		while (vrContext->PollNextEvent(&vr_event, sizeof(vr_event)))
		{
			processVREvent(vr_event);
		}

		// Update tracking device poses...
		vr::VRCompositor()->WaitGetPoses(trackedDevicesPoses, vr::k_unMaxTrackedDeviceCount, NULL, 0);

		for (int nDevice = 0; nDevice<vr::k_unMaxTrackedDeviceCount; nDevice++)
		{
			if (trackedDevicesPoses[nDevice].bPoseIsValid)
			{
				trackedDevicePoseMatrix[nDevice] = convertStemVRMatrixToGLMMatrix(trackedDevicesPoses[nDevice].mDeviceToAbsoluteTracking);
			}
		}

		if (trackedDevicesPoses[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
		{
			HMDPoseMatrix = trackedDevicePoseMatrix[vr::k_unTrackedDeviceIndex_Hmd];
		}
		//model *= glm::rotate(glm::mat4(1.f), glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));	// Rotate along Y
	}

	

	// Update the model matrix to rotate the cube on the Y axis
	float time_2 = (float)SDL_GetTicks();
	/*model *= glm::rotate(glm::mat4(1.f), ((time_2 - time_1) / 1000)*glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));	// Rotate along Y
	time_1 = time_2;*/
}

void VR_Manager::draw()
{
	//glClearColor(1.f, 1.f, 1.f, 1.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, l_frame_buffer);
	glViewport(0, 0, render_width, render_height);
	glEnable(GL_DEPTH_TEST);

	// Make our background white
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//SCENE SHADER
		triggerModel = render_models[1];

	scene_shader->use_Shader();
	if(triggerModel->modelMatrix != nullptr)
	scene_shader->set_mvp_matrix(projectionMatrixLeft*viewMatrixLeft*glm::inverse(HMDPoseMatrix)*convertStemVRMatrixToGLMMatrix(*triggerModel->modelMatrix));//
	else
	scene_shader->set_mvp_matrix(projectionMatrixLeft*viewMatrixLeft*glm::inverse(HMDPoseMatrix)*model);// If we don't apply the hmd_pose_matrix^-1 here the cube will follow the HMD position (kinda UI)
	if(render_models[1] != NULL)render_models[1]->draw();

	glUniform1i(scene_shader->getUniformLocation("hasTexture"), true);
	//quad->Draw();
	/*glBindVertexArray(cube_texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cube_texture);

	

*/
// Draw top cube to frame buffer
	glDrawArrays(GL_TRIANGLES, 0, 30);
	glBindVertexArray(0);
		//SCENE SHADER
	for (int rm_id = 0; rm_id<vr::k_unMaxTrackedDeviceCount; rm_id++)
	{
		if (render_models[rm_id] != NULL)
		{
			
			render_model_shader->use_Shader();
			render_model_shader->set_mvp_matrix(projectionMatrixLeft*viewMatrixLeft*glm::inverse(HMDPoseMatrix)*trackedDevicePoseMatrix[rm_id]);
			//glUniform1i(glGetUniformLocation(render_model_shader->get_shader(),"tex"), 0); // I guess this is not neccessary as it binds texture #0 to the shader parameter by default...

			render_models[rm_id]->draw();
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//////////////////////////////////////////////
	// Render right eye...

	glBindFramebuffer(GL_FRAMEBUFFER, r_frame_buffer);
	glViewport(0, 0, render_width, render_height);
	glEnable(GL_DEPTH_TEST);

	// Make our background white
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	scene_shader->use_Shader();
	if (triggerModel->modelMatrix != nullptr)
		scene_shader->set_mvp_matrix(projectionMatrixRight*viewMatrixRight*glm::inverse(HMDPoseMatrix)*convertStemVRMatrixToGLMMatrix(*triggerModel->modelMatrix));//
	else
		scene_shader->set_mvp_matrix(projectionMatrixRight*viewMatrixRight*glm::inverse(HMDPoseMatrix)*model);// If we don't apply the hmd_pose_matrix^-1 here the cube will follow the HMD position (kinda UI)
	if (render_models[1] != NULL)render_models[1]->draw();
	glUniform1i(scene_shader->getUniformLocation("hasTexture"), true);
	/*scene_shader->use_program();
	scene_shader->set_mvp_matrix(projection_matrix_right*view_matrix_right*glm::inverse(hmd_pose_matrix)*model); // If we don't apply the hmd_pose_matrix^-1 here the cube will follow the HMD position (kinda UI)

	glBindVertexArray(cube_texture);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cube_texture);

	// Draw top cube to frame buffer
	glDrawArrays(GL_TRIANGLES, 0, 30);

	glBindVertexArray(0);*/

	for (int rm_id = 0; rm_id<vr::k_unMaxTrackedDeviceCount; rm_id++)
	{
		if (render_models[rm_id] != NULL)
		{
			render_model_shader->use_Shader();
			render_model_shader->set_mvp_matrix(projectionMatrixRight*viewMatrixRight*glm::inverse(HMDPoseMatrix)*trackedDevicePoseMatrix[rm_id]);
			//glUniform1i(glGetUniformLocation(render_model_shader->get_shader(),"tex"), 0); // I guess this is not neccessary as it binds texture #0 to the shader parameter by default...

			render_models[rm_id]->draw();
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Submit render buffers to HMD

	vr::Texture_t leftEyeTexture = { (void*)(uintptr_t)l_tex_color_buffer, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
	vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)r_tex_color_buffer, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);

	/////////////////////////////////////////////
	// Render what companion window will show
	// In this case we render what left eye is actually seeing

/*	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	screen_shader->use_Shader();

	// Bind default framebuffer and draw contents of our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(square_texture_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, square_texture_ebo);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, l_tex_color_buffer); // Do mind that calls like glBindTexture which change the OpenGL state are relatively expensive, so we must try to keeping them to a minimum
	glUniform1i(glGetUniformLocation(screen_shader->get_shader(), "frame_buffer_tex"), 0);

	glDrawElements(GL_TRIANGLES, sizeof(elements), GL_UNSIGNED_INT, 0);

	glUseProgram(0);
	glBindVertexArray(0);

	// Swap our buffers to make our changes visible
	SDL_GL_SwapWindow(companion_window);

	// Flush and wait for swap
	if (v_blank)
	{
		glFlush();
		glFinish();
	}*/
}

void VR_Manager::exit()
{
	


	for (int rm_id = 0; rm_id<vr::k_unMaxTrackedDeviceCount; rm_id++)
	{
		if (render_models[rm_id] != NULL)
		{
			delete render_models[rm_id];
			render_models[rm_id] = NULL;
		}
	}

	glDeleteBuffers(1, &square_texture_vbo);
	glDeleteBuffers(1, &square_texture_ebo);
	glDeleteVertexArrays(1, &square_texture_vao);

	;

	glDeleteFramebuffers(1, &l_frame_buffer);
	glDeleteRenderbuffers(1, &l_render_buffer_depth_stencil);

	glDisableVertexAttribArray(0);

	vr::VR_Shutdown();
}
