#version 430 core

uniform sampler2D terrainMap;

in vec2 tes_texCoords;
out vec4 fragColor;

void main(void)
{
	float height = texture(terrainMap, tes_texCoords).r;

	vec4 blue = vec4(0.0f, 0.0f, 0.9f, 1.0f);
	vec4 red = vec4(0.9f, 0.35f, 0.0f, 1.0f);

	vec4 color = mix(blue, red, height);

	fragColor = color;
}