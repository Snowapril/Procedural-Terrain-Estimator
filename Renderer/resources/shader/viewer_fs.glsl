#version 430 core

uniform sampler2D textureView;
uniform bool depthRender;

in vec2 texCoords;
out vec4 fragColors;

void main(void)
{
	vec3 color;

	if (depthRender)
	{
		float depth = texture(textureView, texCoords).r;
		
		color = vec3(depth);
	}
	else
	{
		color = texture(textureView, texCoords).rgb;
	}

	fragColors = vec4(color, 1.0);
}