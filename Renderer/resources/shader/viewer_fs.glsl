#version 430 core

uniform sampler2D textureView;
uniform bool depthRender;
uniform float zNear;
uniform float zFar;

in vec2 texCoords;
out vec4 fragColors;

float depthLinearize(float depth)
{
	float z = depth * 2.0 - 1.0;
	return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));
}

void main(void)
{
	vec3 color;
	if (depthRender)
	{
		float depth = texture(textureView, texCoords).r;
		depth = depthLinearize(depth);
		
		color = vec3(depth);
	}
	else
	{
		color = texture(textureView, texCoords).rgb;
	}

	fragColors = vec4(color, 1.0);
}