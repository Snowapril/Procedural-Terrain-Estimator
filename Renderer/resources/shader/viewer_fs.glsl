#version 430 core

uniform sampler2D textureView;
uniform bool depthRender;
uniform float near;
uniform float far;

in vec2 texCoords;
out vec4 fragColors;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // back to NDC 
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main(void)
{
	vec3 color;

	if (depthRender)
	{
		float depth = texture(textureView, texCoords).r;
		depth = LinearizeDepth(depth);

		color = vec3(depth);
	}
	else
	{
		color = texture(textureView, texCoords).rgb;
	}

	fragColors = vec4(color, 1.0);
}