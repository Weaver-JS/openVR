#version 420

in vec3 in_position;
in vec3 in_color;
in vec2 uv;

out vec3 Color;
out vec2 uvs;
uniform mat4 mvp;



void main()
{
	Color = in_color;
	uvs = uv;
	gl_Position = mvp * vec4(in_position,1.0);
	
}