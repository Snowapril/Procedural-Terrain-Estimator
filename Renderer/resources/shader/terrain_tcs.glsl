#version 430 core

layout(vertices = 4) out;

in float vs_tessLevel[];
in vec2 vs_texCoords[];

out vec2 tcs_texCoords[];
out float tcs_tessLevel[];

void main(void)
{
	float tessLevelOuter = pow(2.0, vs_tessLevel[gl_InvocationID]);

	gl_TessLevelOuter[0] = tessLevelOuter;
	gl_TessLevelOuter[1] = tessLevelOuter;
	gl_TessLevelOuter[2] = tessLevelOuter;
	gl_TessLevelOuter[3] = tessLevelOuter;

	gl_TessLevelInner[0] = (gl_TessLevelOuter[0] + gl_TessLevelOuter[3]) * 0.5;
	gl_TessLevelInner[1] = (gl_TessLevelOuter[1] + gl_TessLevelOuter[2]) * 0.5;

	tcs_texCoords[gl_InvocationID] = vs_texCoords[gl_InvocationID];
	tcs_tessLevel[gl_InvocationID] = vs_tessLevel[gl_InvocationID];

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}