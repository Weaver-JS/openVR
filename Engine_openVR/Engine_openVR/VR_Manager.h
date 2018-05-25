#pragma once
/*#include <SDL2\SDL.h>
#include <OpenVR\openvr.h>
#include <OpenVR\openvr_capi.h>
#include <OpenVR\openvr_driver.h>*/
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "RenderModel.h"
#include "Constants.h"
#include "models.h"
#include "TestShape.h"
class VR_Manager
{
private:
	vr::IVRSystem *vrContext;
	vr::IVRRenderModels * vrModels;
	vr::IVRCompositor * vrCompositor;
	Shader* render_model_shader;
	Shader* screen_shader;
	Shader* scene_shader;
	uint32_t render_width, render_height;
	TestShape* quad;
	glm::mat4 model;
	RenderModel * triggerModel;
	float time;
	std::string m_strDriver;
	std::string m_strDisplay;
	float fNearZ, fFarZ;
	int base_stations_count;
	RenderModel * render_models[vr::k_unMaxTrackedDeviceCount]; // Modelos a renderizar.
	vr::TrackedDevicePose_t trackedDevicesPoses[vr::k_unMaxTrackedDeviceCount]; //Objetos trackeables.
	
	/* Matrices*/
	glm::mat4 trackedDevicePoseMatrix[vr::k_unMaxTrackedDeviceCount]; //Matrices para los objetos trackeables
	glm::mat4 HMDPoseMatrix; //Matriz para el casco de realidad virtual
	glm::mat4 projectionMatrixLeft, projectionMatrixRight; //Matriz de proyección tanto para el ojo izquierdo como el derecho
	glm::mat4 viewMatrixLeft, viewMatrixRight; //View Matrix Izq y derecha
	/*Matrices*/
	GLuint square_texture_vbo, square_texture_vao, square_texture_ebo;

	GLuint squareTextureVBO, squareTextureVAO, squareTextureEBO;
	// Buffer ids for the left and rigth eyes frame buffer and stuff
	GLuint l_frame_buffer, l_tex_color_buffer, l_render_buffer_depth_stencil;
	GLuint r_frame_buffer, r_tex_color_buffer, r_render_buffer_depth_stencil;

	int initOpenVR();
	int initOpenGL();
	void processVREvent(const vr::VREvent_t & vrEvent);
	bool setup_render_model(vr::TrackedDeviceIndex_t tracked_device);
	bool setup_scene_buffers();
	bool setup_frame_buffer(GLsizei width, GLsizei height, GLuint & frame_buffer, GLuint & render_buffer_depth_stencil, GLuint & tex_color_buffer);
	glm::mat4 convertStemVRMatrixToGLMMatrix(const vr::HmdMatrix34_t &matPose);
	glm::mat4 getProjectionMatrix(const vr::Hmd_Eye eye);
	glm::mat4 getViewMatrix(const vr::Hmd_Eye eye);

	//Utils

	std::string GetTrackedDeviceClassString(vr::ETrackedDeviceClass deviceClass);
	std::string GetTrackedDeviceString(vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL);

public:
	VR_Manager();
	~VR_Manager();
	int init();
	void update();
	void draw();
	void exit();
	void setSceneShader(Shader & scene_shader);
};

