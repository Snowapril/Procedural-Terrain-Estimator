#version 430 core

in vec3 texCoords;
out vec4 fragColor;

uniform samplerCube cubeMap;

const vec4 skycolor = vec4(0.5, 0.5, 0.5, 1.0);

const float lowerLimit = 0.0;
const float upperLimit = 0.1;

void main(void)
{
	vec4 envColor = texture(cubeMap, texCoords);

	float factor = (texCoords.y - lowerLimit) / (upperLimit - lowerLimit);

	factor = clamp(factor, 0.0, 1.0);
	
	fragColor = mix(skycolor, envColor, factor);
}