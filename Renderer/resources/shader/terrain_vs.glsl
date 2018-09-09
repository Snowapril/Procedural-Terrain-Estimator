#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in float aTessLevel;

layout(std140) uniform VP
{
	mat4 view;
	mat4 project;
};

out float vs_tessLevel;
out vec2 vs_texCoords;

uniform vec3 originPos;
uniform vec2 terrainScale;

vec2 calculateTexCoords(vec3 vertex)
{
	return ( vec2(vertex.x, vertex.z) + originPos.xz + terrainScale / 2.0 ) / terrainScale;
}

void main(void)
{
	vs_texCoords = calculateTexCoords(aPos);
	vs_tessLevel = aTessLevel;

	gl_Position = vec4(aPos, 1.0);
}