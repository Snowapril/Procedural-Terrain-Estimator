#version 430 core

in vec3 texCoords;
out vec4 fragColor;

uniform samplerCube cubeMap;

uniform vec3 skycolor = vec3(0.5, 0.5, 0.5);

uniform float lowerLimit = 0.0;
uniform float upperLimit = 0.1;

void main(void)
{
	vec4 envColor = texture(cubeMap, texCoords);

	float factor = (texCoords.y - lowerLimit) / (upperLimit - lowerLimit);

	factor = clamp(factor, 0.0, 1.0);
	
	fragColor = mix(vec4(skycolor, 1.0), envColor, factor);
}