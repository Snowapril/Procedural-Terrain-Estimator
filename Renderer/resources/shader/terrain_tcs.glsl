#version 430 core

layout(vertices = 4) out;

void main(void)
{
	gl_TessLevelOuter[0] = 16;
	gl_TessLevelOuter[1] = 16;
	gl_TessLevelOuter[2] = 16;
	gl_TessLevelOuter[3] = 16;

	gl_TessLevelInner[0] = 8;
	gl_TessLevelInner[1] = 8;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}