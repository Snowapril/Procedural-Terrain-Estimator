#version 430 core

layout(location = 0) in vec3 aPos;

layout (std140) uniform VP
{
	mat4 view;
	mat4 projection;
};

uniform mat4 model;
uniform float tileScale;

out VS_OUT{
	vec2 terrainTexCoords;
} vs_out;

void main(void)
{
	
}