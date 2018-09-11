#version 430 core

in vec2 TexCoords;
out vec4 fragColors;

uniform sampler2D heightMap;

void main(void)
{
	float height = texture(heightMap, TexCoords).r;

	const ivec3 offset = ivec3(-1, 0, 1);
	const vec2 size = vec2(2.0, 0.0);

	float hL = textureOffset(heightMap, TexCoords, offset.xy);
	float hR = textureOffset(heightMap, TexCoords, offset.zy);
	float hD = textureOffset(heightMap, TexCoords, offset.yx);
	float hU = textureOffset(heightMap, TexCoords, offset.yz);
	
	vec3 va = normalize(vec3(size.xy, hR - hL));
	vec3 vb = normalize(vec3(size.yx, hU - hD));

	vec3 normal = normalize(cross(va, vb));

	fragColors = vec4(normal, height); 
}