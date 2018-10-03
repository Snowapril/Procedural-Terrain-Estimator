#version 430 core

in vec2 TexCoords;
out vec4 fragColors;

uniform sampler2D heightMap;
uniform float terrainMaxHeight;

void main(void)
{
	float height = texture(heightMap, TexCoords).r;

	const ivec3 offset = ivec3(-1, 0, 1);
	const vec2 size = vec2(2.0, 0.0);

	float hL = textureOffset(heightMap, TexCoords, offset.xy).r * terrainMaxHeight;
	float hR = textureOffset(heightMap, TexCoords, offset.zy).r * terrainMaxHeight;
	float hD = textureOffset(heightMap, TexCoords, offset.yx).r * terrainMaxHeight;
	float hU = textureOffset(heightMap, TexCoords, offset.yz).r * terrainMaxHeight;
	
	vec3 normal = normalize(vec3(hR - hL, 2, hU - hD));
	normal.xz = normal.xz * 0.5 + 0.5;

	fragColors = vec4(normal.xzy, height); 
}		