#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in float aTessLevel;

out float vs_tessLevel;
out vec2 vs_texCoords;
out vec2 vs_tileCoords;

uniform vec3 originPos;
uniform vec2 terrainScale;

const float TILE_SIZE = 16.0;

vec2 calculateTexCoords(vec3 vertex)
{
	return ( vec2(vertex.x, vertex.z) + originPos.xz + terrainScale / 2.0 ) / terrainScale;
}

void main(void)
{
	vs_texCoords = calculateTexCoords(aPos);
	vs_tessLevel = aTessLevel;
	vs_tileCoords = (aPos.xz / terrainScale + 0.5) * TILE_SIZE;

	gl_Position = vec4(aPos, 1.0);
}