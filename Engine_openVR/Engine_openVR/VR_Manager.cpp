#include "VR_Manager.h"



int VR_Manager::initOpenVR()
{
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
	return glm::mat4();
}

glm::mat4 VR_Manager::getProjectionMatrix(const vr::Hmd_Eye eye)
{
	return glm::mat4();
}

glm::mat4 VR_Manager::getViewMatrix(const vr::Hmd_Eye eye)
{
	return glm::mat4();
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

	m_strDriver = "No Driver";
	m_strDisplay = "";
	
	return initOpenVR();
	
}
