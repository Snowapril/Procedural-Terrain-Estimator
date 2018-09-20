#version 430 core

layout(vertices = 4) out;

layout (std140) uniform VP
{
	mat4 view;
	mat4 project;
};

in float vs_tessLevel[];
in vec2 vs_texCoords[];
in vec2 vs_tileCoords[];

out vec2 tcs_texCoords[];
out vec2 tcs_tileCoords[];

uniform sampler2D terrainMap;
uniform float terrainMaxHeight;

float dlodCameraDistance(vec4 p0, vec4 p1, vec2 t0, vec2 t1)
{
	float height = texture(terrainMap, t0).a;
	p0.y = height * terrainMaxHeight;
	height = texture(terrainMap, t1).a;
	p1.y = height * terrainMaxHeight;

	vec4 view0 = view * p0;
	vec4 view1 = view * p1;

	float MinDepth = 1.0;
	float MaxDepth = 1000.0;

	float d0 = clamp((abs(p0.z) - MinDepth) / (MaxDepth - MinDepth), 0.0, 1.0); 
	float d1 = clamp((abs(p1.z) - MinDepth) / (MaxDepth - MinDepth), 0.0, 1.0);

	float t = mix(32, 4, (d0 + d1) * 0.5);

	/*
	if (t <= 2.0)
	{
		return 2.0;
	}
	if (t <= 4.0)
	{
		return 4.0;
	}
	if (t <= 8.0)
	{
		return 8.0;
	}
	if (t <= 16.0)
	{
		return 16.0;
	}
	if (t <= 32.0)
	{
		return 32.0;
	}
	*/
	
	highp int temp = int(t - 1.0);

	temp |= temp >> 1;
	temp |= temp >> 2;
	temp |= temp >> 4;
	temp |= temp >> 8;

	t = float(temp + 1);

	return t;
}

void main(void)
{
	gl_TessLevelOuter[0] = dlodCameraDistance(gl_in[3].gl_Position, gl_in[0].gl_Position, vs_texCoords[0], vs_texCoords[0]);
	//if (vs_tessLevel[0] == 2.0)
	//	gl_TessLevelOuter[0] = max(2.0, gl_TessLevelOuter[0] * 0.5);
	gl_TessLevelOuter[0] = max(2.0, gl_TessLevelOuter[0] / vs_tessLevel[0]);

	gl_TessLevelOuter[1] = dlodCameraDistance(gl_in[0].gl_Position, gl_in[1].gl_Position, vs_texCoords[0], vs_texCoords[0]);
	//if (vs_tessLevel[1] == 2.0)
	//	gl_TessLevelOuter[1] = max(2.0, gl_TessLevelOuter[1] * 0.5);
	gl_TessLevelOuter[1] = max(2.0, gl_TessLevelOuter[1] / vs_tessLevel[1]);

	gl_TessLevelOuter[2] = dlodCameraDistance(gl_in[1].gl_Position, gl_in[2].gl_Position, vs_texCoords[0], vs_texCoords[0]);
	//if (vs_tessLevel[2] == 2.0)
	//	gl_TessLevelOuter[2] = max(2.0, gl_TessLevelOuter[2] * 0.5);
	gl_TessLevelOuter[2] = max(2.0, gl_TessLevelOuter[2] / vs_tessLevel[2]);

	gl_TessLevelOuter[3] = dlodCameraDistance(gl_in[2].gl_Position, gl_in[3].gl_Position, vs_texCoords[0], vs_texCoords[0]);
	//if (vs_tessLevel[3] == 2.0)
	//	gl_TessLevelOuter[3] = max(2.0, gl_TessLevelOuter[3] * 0.5);
	gl_TessLevelOuter[3] = max(2.0, gl_TessLevelOuter[3] / vs_tessLevel[3]);

	gl_TessLevelInner[0] = (gl_TessLevelOuter[0] + gl_TessLevelOuter[3]) * 0.5;
	gl_TessLevelInner[1] = (gl_TessLevelOuter[1] + gl_TessLevelOuter[2]) * 0.5;

	tcs_texCoords[gl_InvocationID] = vs_texCoords[gl_InvocationID];
	tcs_tileCoords[gl_InvocationID] = vs_tileCoords[gl_InvocationID];
	
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}	