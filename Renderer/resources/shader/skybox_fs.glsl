#version 430 core

in vec3 texCoords;
out vec4 fragColor;

uniform samplerCube cubeMap;

const float GAMMA = 2.2;

void main(void)
{
	vec3 envColor = texture(cubeMap, texCoords).rgb;

	envColor = envColor / (envColor + vec3(1.0));
	envColor = pow(envColor, vec3(1.0 / GAMMA));

	fragColor = vec4(envColor, 1.0);
}