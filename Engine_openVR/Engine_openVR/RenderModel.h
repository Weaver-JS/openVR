#pragma once
#include <GL\glew.h>

#include <iostream>
#include <SDL2\SDL.h>
#include <GL\GL.h>
#include <OpenVR\openvr.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

class RenderModel
{
public:
	RenderModel();
	RenderModel(const char* device);
	~RenderModel();

	bool init();
	void update();
	void draw();
	void clean_up();

	GLuint get_texture_id();
	vr::HmdMatrix34_t * modelMatrix;
private:
	GLuint render_model_vao, render_model_vbo, render_model_ebo;
	GLuint render_model_texture;
	GLsizei vertex_count;

	char* name;

	bool setup_buffers(const vr::RenderModel_t &, const vr::RenderModel_TextureMap_t&);
};

