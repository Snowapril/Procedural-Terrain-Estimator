#version 430 core

layout(quads, fractional_even_spacing) in;

uniform mat4 sunMVPMatrix;

in vec2 tcs_texCoords[];
in vec2 tcs_tileCoords[];

uniform sampler2D terrainMap;
uniform float terrainMaxHeight;
uniform vec4 clipPlane;

uniform mat4 view;
uniform mat4 project;

uniform bool enableVP;

vec4 interpolate4(vec4 v0, vec4 v1, vec4 v2, vec4 v3)
{
	vec4 a = mix(v0, v1, gl_TessCoord.x);
	vec4 b = mix(v2, v3, gl_TessCoord.x);

	return mix(a, b, gl_TessCoord.y);
}

vec2 interpolate2(vec2 v0, vec2 v1, vec2 v2, vec2 v3)
{
	vec2 a = mix(v0, v1, gl_TessCoord.x);
	vec2 b = mix(v2, v3, gl_TessCoord.x);

	return mix(a, b, gl_TessCoord.y);
}

void main(void)
{
	vec2 terrainTexCoords = interpolate2(tcs_texCoords[0], tcs_texCoords[1], tcs_texCoords[2], tcs_texCoords[3]);
	float height = texture(terrainMap, terrainTexCoords).a;

	gl_Position = interpolate4(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);
	gl_Position.y = height * terrainMaxHeight;

	gl_ClipDistance[0] = dot(gl_Position, clipPlane);

	if (enableVP)
	{
		gl_Position = project * view * gl_Position;
	}
	else
	{
		gl_Position = sunMVPMatrix * gl_Position;
	}
}