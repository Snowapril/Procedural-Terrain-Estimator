#version 430 core

layout(vertices = 4) out;

in uint vs_tessLevel[];
out uint tcs_tessLevel[];

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

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}