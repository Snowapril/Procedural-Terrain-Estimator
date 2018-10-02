#version 430 core

in vec2 texCoords;
out vec4 fragColors;

uniform sampler2D hdrBuffer;

void main(void)
{
	const float GAMMA = 1.8;
	vec3 hdrColor = texture(hdrBuffer, texCoords).rgb;

	vec3 result = hdrColor / (hdrColor + vec3(1.0));

	result = pow(hdrColor, vec3(1.0 / GAMMA));

	fragColors = vec4(result, 1.0);
}