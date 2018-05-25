#version 150

in vec3 Color;
in vec2 uvs;
out vec4 outColor;

uniform bool hasTexture;
uniform sampler2D in_texture;

void main()
{
	vec3 tempColor = Color ;
	if(hasTexture)
	{
		 outColor = texture(in_texture, uvs);
		//tempColor += texture(in_texture,uvs).rgb;
	}
	 outColor = texture(in_texture, uvs);
	//	outColor = vec4(tempColor,1.0f);
}