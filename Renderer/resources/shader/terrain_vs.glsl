#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in uint aTessLevel;

out uint vs_tessLevel;
out vec2 vs_texCoords;

uniform vec2 originPos;
uniform vec2 terrainScale;

vec2 calculateTexCoords(vec4 vertex)
{
	return vec2(0.0);
}

void main(void)
{
	vs_tessLevel = aTessLevel;
	vs_texCoords = calculateTexCoords(aPos);

	gl_Position = vec4(aPos, 1.0);
}