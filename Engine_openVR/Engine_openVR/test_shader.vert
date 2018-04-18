#version 420

uniform vec3 position;
in vec3 in_position;
in vec3 in_color;
in vec2 uv;

out vec3 Color;
out vec2 uvs;
uniform mat4 mvpMatrix;



void main()
{
	Color = position;
	uvs = uv;
	gl_Position = mvpMatrix * vec4(in_position,1.0);
	
}