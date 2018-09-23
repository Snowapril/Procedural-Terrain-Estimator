#version 430 core

layout (location = 0) in vec2 aPos;

uniform vec3 viewPos;

out vec4 clipPosition;
out vec2 texCoords;
out vec3 toCameraVector;
out vec3 toLightVector;

uniform vec3 lightPosition;

layout (std140) uniform VP 
{
    mat4 view;
    mat4 project;
};

uniform mat4 model;

const float tiling = 6.0;

void main(void) 
{
	vec4 worldPosition = model * vec4(aPos.x, 0.0, aPos.y, 1.0);
	clipPosition = project * view * worldPosition;
	gl_Position = clipPosition;

	texCoords = (aPos * 0.5 + 0.5) * tiling;
	toCameraVector = viewPos - worldPosition.xyz;
	toLightVector = worldPosition.xyz - lightPosition;
}