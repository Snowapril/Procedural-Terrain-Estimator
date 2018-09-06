#version 430 core

layout(vertices = 4) out;

in uint vs_tessLevel[];
in vec2 vs_texCoords[];

out vec2 tcs_texCoords[];

void main(void)
{
	uint tessLevelOuter = 1 << vs_tessLevel[0];
	uint tessLevelInner = tessLevelOuter >> 1;

	gl_TessLevelOuter[0] = tessLevelOuter;
	gl_TessLevelOuter[1] = tessLevelOuter;
	gl_TessLevelOuter[2] = tessLevelOuter;
	gl_TessLevelOuter[3] = tessLevelOuter;

	gl_TessLevelInner[0] = tessLevelInner;
	gl_TessLevelInner[1] = tessLevelInner;

	tcs_texCoords[gl_InvocationID] = vs_texCoords[gl_InvocationID];
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}