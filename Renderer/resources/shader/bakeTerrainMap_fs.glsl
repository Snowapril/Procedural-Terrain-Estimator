#version 430 core

in vec2 TexCoords;
out vec4 fragColors;

uniform sampler2D heightMap;
uniform float terrainMaxHeight;
uniform float terrainHeightOffset;

void main(void)
{
	float height = texture(heightMap, TexCoords).r;

	const ivec3 offset = ivec3(-1, 0, 1);
	const vec2 size = vec2(2.0, 0.0);

	float hL = textureOffset(heightMap, TexCoords, offset.xy).r * terrainMaxHeight + terrainHeightOffset;
	float hR = textureOffset(heightMap, TexCoords, offset.zy).r * terrainMaxHeight + terrainHeightOffset;
	float hD = textureOffset(heightMap, TexCoords, offset.yx).r * terrainMaxHeight + terrainHeightOffset;
	float hU = textureOffset(heightMap, TexCoords, offset.yz).r * terrainMaxHeight + terrainHeightOffset;
	
	vec3 va = normalize(vec3(size.xy, hR - hL));
	vec3 vb = normalize(vec3(size.yx, hU - hD));
		
	vec3 normal = normalize(cross(va, vb));

	//vec3 normal = normalize(vec3(hR - hL, -2, hU - hD));

	fragColors = vec4(normal, height); 
}