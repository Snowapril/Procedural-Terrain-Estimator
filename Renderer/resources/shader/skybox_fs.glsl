#version 430 core

in vec3 texCoords;
out vec4 fragColor;

uniform samplerCube cubeMap;

void main(void)
{
    fragColor = texture(cubeMap, texCoords);
}