#version 430 core

layout(quads, fractional_even_spacing) in;

layout (std140) uniform VP
{
	mat4 view;
	mat4 project;
};

in vec2 tcs_texCoords[];

uniform sampler2D terrainMap;
uniform float terrainMaxHeight;
uniform float terrainHeightOffset;

vec2 interpolate(vec2 v0, vec2 v1, vec2 v2, vec2 v3)
{
	vec2 a = mix(v0, v1, gl_TessCoord.x);
	vec2 b = mix(v3, v2, gl_TessCoord.x);

	return mix(a, b, gl_TessCoord.y);
}

void main(void)
{
	vec4 a = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
	vec4 b = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);

	vec2 terrainTexCoords = interpolate(tcs_texCoords[0], tcs_texCoords[1], tcs_texCoords[2], tcs_texCoords[3]);
	float height = texture(terrainMap, terrainTexCoords).r;

	gl_Position = mix(a, b, gl_TessCoord.y);
	gl_Position.y = height * terrainMaxHeight + terrainHeightOffset;

	gl_Position = project * view * gl_Position;
}