#ifndef TERRAIN_SHADER_CODE
#define TERRAIN_SHADER_CODE

#ifdef _DEBUG

constexpr const char BAKETERRAINMAP_VS[] = R"glsl(
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
out vec2 TexCoords;
uniform mat4 view;
uniform mat4 projection;
void main(void)
{
	TexCoords = aTexCoords;
	gl_Position = projection * view * vec4(aPos, 1.0);
}
)glsl";

constexpr const char BAKETERRAINMAP_FS[] = R"glsl(
#version 430 core

in vec2 TexCoords;
out vec4 fragColors;

uniform sampler2D heightMap;
uniform float terrainMaxHeight;

void main(void)
{
	float height = texture(heightMap, TexCoords).r;

	const ivec3 offset = ivec3(-1, 0, 1);
	const vec2 size = vec2(2.0, 0.0);

	float hL = textureOffset(heightMap, TexCoords, offset.xy).r * terrainMaxHeight;
	float hR = textureOffset(heightMap, TexCoords, offset.zy).r * terrainMaxHeight;
	float hD = textureOffset(heightMap, TexCoords, offset.yx).r * terrainMaxHeight;
	float hU = textureOffset(heightMap, TexCoords, offset.yz).r * terrainMaxHeight;
	
	vec3 normal = normalize(vec3(hL - hR, 2, hD - hU));
	normal.xz = normal.xz * 0.5 + 0.5;

	fragColors = vec4(normal.xzy, height); 
}		
)glsl";

constexpr const char TERRAIN_VS[] = R"glsl(
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in float aTessLevel;

out float vs_tessLevel;
out vec2 vs_texCoords;
out vec2 vs_tileCoords;

uniform vec3 originPos;
uniform vec2 terrainScale;

vec2 calculateTexCoords(vec3 vertex)
{
	return ( vec2(vertex.x, vertex.z) + originPos.xz + terrainScale / 2.0 ) / terrainScale;
}

void main(void)
{
	vs_texCoords = calculateTexCoords(aPos);
	vs_tessLevel = aTessLevel;
	vs_tileCoords = aPos.xz / (terrainScale / 32.0);

	gl_Position = vec4(aPos, 1.0);
}	
)glsl";

constexpr const char TERRAIN_TCS[] = R"glsl(
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
)glsl";

constexpr const char TERRAIN_TES[] = R"glsl(
#version 430 core

layout(quads, fractional_even_spacing) in;

layout(std140) uniform VP
{
	mat4 view;
	mat4 project;
};

in vec2 tcs_texCoords[];
in vec2 tcs_tileCoords[];

out vec2 tes_texCoords;
out vec2 tes_tessCoords;
out vec2 tes_tileCoords;
out float visibility;

uniform sampler2D terrainMap;
uniform float terrainMaxHeight;
uniform vec4 clipPlane;

const float density = 0.007;
const float gradient = 1.5;

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

	tes_texCoords = terrainTexCoords;
	tes_tessCoords = gl_TessCoord.xy;

	tes_tileCoords = interpolate2(tcs_tileCoords[0], tcs_tileCoords[1], tcs_tileCoords[2], tcs_tileCoords[3]);

	gl_Position = interpolate4(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);
	gl_Position.y = height * terrainMaxHeight;

	gl_ClipDistance[0] = dot(gl_Position, clipPlane); 
	 
	vec4 positionRelativeToCam = view * gl_Position;
	gl_Position = project * positionRelativeToCam;

	float distance = length(positionRelativeToCam);
	visibility = exp(-pow(distance * density, gradient));
	visibility = clamp(visibility, 0.0, 1.0);
}
)glsl";

constexpr const char TERRAIN_FS[] = R"glsl(
#version 430 core

in vec2 tes_texCoords;
in vec2 tes_tessCoords;
in vec2 tes_tileCoords;
in float visibility;

out vec4 fragColor;

uniform sampler2D terrainMap;

uniform sampler2D splatMap;
uniform sampler2D dirtTexture;
uniform sampler2D rockTexture;
uniform sampler2D grassTexture;

uniform vec3 wireColor;

const vec4 skycolor = vec4(0.5, 0.5, 0.5, 1.0);

void main(void)
{
	vec4 terrain = texture(terrainMap, tes_texCoords);
	
	float height = terrain.w;
	vec3 normal = normalize(terrain.xyz);
	normal.xz = normal.xz * 2.0 - 1.0;

	vec4 mixmap = texture(splatMap, tes_texCoords);
	
	vec3 dirt = texture(dirtTexture, tes_tileCoords).rgb;
	vec3 rock = texture(rockTexture, tes_tileCoords).rgb;
	vec3 water = texture(grassTexture, tes_tileCoords).rgb;
	
	vec3 red = vec3(1.0, 0.0, 0.0);
	vec3 blue = vec3(0.0, 0.0, 1.0);
	vec3 mixColor = mix(blue, red, height);

	vec3 finalColor = mix(dirt, water, mixmap.b);
	finalColor = mix(finalColor, rock, mixmap.r);// *0.0 + 1.0 * mixColor;

	vec3 lightDir = normalize(vec3(1.f, 1.f, 0.f));
	vec3 lightDiffuse = vec3(0.9);
	float diff = max(dot(lightDir, normal), 0.0);
	
	vec3 diffuse = diff * lightDiffuse;
	vec3 ambient = vec3(0.35);
	
	finalColor = (ambient + diffuse) * finalColor;

	fragColor = vec4(finalColor * wireColor, 1.0);
	//fragColor = mix(skycolor, fragColor, visibility);
}
)glsl";

#endif

#endif