#version 430 core

layout(quads, fractional_even_spacing) in;

layout (std140) uniform VP
{
	mat4 view;
	mat4 project;
};

in uint tcs_tessLevel[];
flat out uint tes_tessLevel;

void main(void)
{
	vec4 a = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
	vec4 b = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);

	tes_tessLevel = tcs_tessLevel[0];

	gl_Position = project * view * mix(a, b, gl_TessCoord.y);
}