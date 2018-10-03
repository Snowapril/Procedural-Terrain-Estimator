#version 430 core

in vec3 texCoords;
out vec4 fragColor;

uniform samplerCube cubeMap;

void main(void)
{
	vec3 envColor = texture(cubeMap, texCoords).rgb;

	fragColor = vec4(envColor, 1.0);
}