#version 150

in vec3 in_position;
in vec3 in_color;

out vec3 Color;

uniform mat4 mvpMatrix;


void main()
{
	Color = in_color;
 	gl_Position = mvpMatrix * vec4(in_position,1.0);
}