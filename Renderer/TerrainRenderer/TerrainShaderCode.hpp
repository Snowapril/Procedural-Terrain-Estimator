#ifndef TERRAIN_SHADER_CODE
#define TERRAIN_SHADER_CODE

#ifndef _DEBUG

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
	
	vec3 normal = normalize(vec3(hR - hL, 2, hU - hD));
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

uniform float tileSize = 32.0;

vec2 calculateTexCoords(vec3 vertex)
{
	return ( vec2(vertex.x, vertex.z) + originPos.xz + terrainScale / 2.0 ) / terrainScale;
}

void main(void)
{
	vs_texCoords = calculateTexCoords(aPos);
	vs_tessLevel = aTessLevel;
	vs_tileCoords = (aPos.xz / terrainScale + 0.5) * tileSize;

	gl_Position = vec4(aPos, 1.0);
}
)glsl";

constexpr const char TERRAIN_TCS[] = R"glsl(
#version 430 core

layout(vertices = 4) out;

uniform mat4 view;
uniform mat4 project;

in float vs_tessLevel[];
in vec2 vs_texCoords[];
in vec2 vs_tileCoords[];

out vec2 tcs_texCoords[];
out vec2 tcs_tileCoords[];

uniform sampler2D terrainMap;
uniform float terrainMaxHeight;

uniform float minDepth;
uniform float maxDepth;

float dlodCameraDistance(vec4 p0, vec4 p1, vec2 t0, vec2 t1)
{
	float height = texture(terrainMap, t0).a;
	p0.y = height * terrainMaxHeight;
	height = texture(terrainMap, t1).a;
	p1.y = height * terrainMaxHeight;

	vec4 view0 = view * p0;
	vec4 view1 = view * p1;

	float d0 = clamp((abs(p0.z) - minDepth) / (maxDepth - minDepth), 0.0, 1.0);
	float d1 = clamp((abs(p1.z) - minDepth) / (maxDepth - minDepth), 0.0, 1.0);

	float t = mix(32, 2, (d0 + d1) * 0.5);

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

in vec2 tcs_texCoords[];
in vec2 tcs_tileCoords[];

out vec2 tes_texCoords;
out vec2 tes_tileCoords;

uniform sampler2D terrainMap;
uniform float terrainMaxHeight;


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

	tes_tileCoords = interpolate2(tcs_tileCoords[0], tcs_tileCoords[1], tcs_tileCoords[2], tcs_tileCoords[3]);

	gl_Position = interpolate4(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);
	gl_Position.y = height * terrainMaxHeight;
}
)glsl";

constexpr const char TERRAIN_GS[] = R"glsl(
#version 430 core

/*
reference :
http://developer.download.nvidia.com/SDK/10/direct3d/Source/SolidWireframe/Doc/SolidWireframe.pdf
*/

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 view;
uniform mat4 project;
uniform mat4 sunMVPMatrix;
uniform vec4 clipPlane;
uniform vec2 viewportSize;

in vec2 tes_texCoords[];
in vec2 tes_tileCoords[];

out vec3 gs_fragPos;
out vec3 gs_shadowCoords;
out float gs_visibility;
out vec2 gs_texCoords;
out vec2 gs_tileCoords;
out vec3 gs_distToEdges;

const float density = 0.00025;
uniform float gradient = 2.2;

vec2 projectToViewportSpace(vec4 position, mat4 vp)
{
	vec4 clip = vp * position;
	clip.xy /= clip.w;
	// -1.0 ~ 1.0 -> 0.0 ~ 1.0 -> width ~ height
	clip.xy = (clip.xy * 0.5 + 0.5) * viewportSize;
	return clip.xy;
}

void emitVertex(int i, vec3 distToEdges)
{
	gs_fragPos = gl_in[i].gl_Position.xyz;
	gs_shadowCoords = (sunMVPMatrix * gl_in[i].gl_Position).xyz;

	vec4 positionRelativeToCam = view * gl_in[i].gl_Position;

	float distance = length(positionRelativeToCam);
	gs_visibility = exp(-pow(distance * density, gradient));
	gs_visibility = clamp(gs_visibility, 0.0, 1.0);

	gs_texCoords = tes_texCoords[i];
	gs_tileCoords = tes_tileCoords[i];
	gs_distToEdges = distToEdges;

	gl_ClipDistance[0] = dot(gl_in[i].gl_Position, clipPlane);
	gl_Position = project * positionRelativeToCam;

	EmitVertex();
}

void main(void)
{
	mat4 vp = project * view;
	
	vec2 p0 = projectToViewportSpace(gl_in[0].gl_Position, vp);
	vec2 p1 = projectToViewportSpace(gl_in[1].gl_Position, vp);
	vec2 p2 = projectToViewportSpace(gl_in[2].gl_Position, vp);

	vec2 e0 = p1 - p0;
	vec2 e1 = p2 - p1;
	vec2 e2 = p2 - p0;

	// cross((b-a), (c-a)).
	// cross( (e1 - e0), (e2 - e0) );
	// ( e1.x - e0.x, e1.y - e0.y, e1.z - e0.z) x (e2.x - e0.x, e2.y - e0.y, e2.z - e0.z)
	// e1.y * e2.z - e1.y * e0.z - e0.y * e2.z + e0.z * e0.y -  .....

	float doubleTriangleArea = abs((e0.x * e2.y) - (e0.y * e2.x));

	float h0 = doubleTriangleArea / length(e0);
	float h1 = doubleTriangleArea / length(e1);
	float h2 = doubleTriangleArea / length(e2);

	emitVertex(0, vec3(0, h1, 0));
	emitVertex(1, vec3(0, 0, h2));
	emitVertex(2, vec3(h0, 0, 0));

	EndPrimitive();
}	
)glsl";

constexpr const char TERRAIN_FS[] = R"glsl(
#version 430 core


in vec2 gs_texCoords;
in vec2 gs_tileCoords;
in vec3 gs_fragPos;
in vec3 gs_shadowCoords;
in vec3 gs_distToEdges;

in float gs_visibility;

out vec4 fragColor;

uniform sampler2D terrainMap;
uniform sampler2D splatMap;
uniform sampler2D dirtTexture;
uniform sampler2D rockTexture;
uniform sampler2D grassTexture;
uniform sampler2D wetDirtTexture;
uniform sampler2D shadowMap;
uniform bool enableWireframe;

struct DirLight {
	vec3 direction;
	vec3 color;
};

uniform DirLight dirLight;

uniform vec3 skycolor = vec3(0.5, 0.5, 0.5);

//float DistributionGGX(vec3 N, vec3 H, float roughness);
//float GeometrySchlickGGX(float NdotV, float roughness);
//float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
//vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

float calculateShadow(vec3 shadowCoords);

void main(void)
{
	vec4 terrain = texture(terrainMap, gs_texCoords);

	float height = terrain.w;
	vec3 normal = terrain.xzy;
	normal.xz = normal.xz * 2.0 - 1.0;

	normal = normalize(normal);

	vec4 mixmap = texture(splatMap, gs_texCoords);

	vec3 dirt = texture(dirtTexture, gs_tileCoords).rgb;
	vec3 rock = texture(rockTexture, gs_tileCoords).rgb;
	vec3 grass = texture(grassTexture, gs_tileCoords).rgb;
	vec3 wetDirt = texture(wetDirtTexture, gs_tileCoords).rgb;

	vec3 finalColor = mix(dirt, grass, mixmap.g);
	finalColor = mix(finalColor, wetDirt, mixmap.b);
	finalColor = mix(finalColor, rock, mixmap.r);
	vec3 ambient = 0.005 * finalColor;

	vec3 lightDir = normalize(dirLight.direction);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = dirLight.color * diff * finalColor;
	
	// attenuation (use quadratic as we have gamma correction)
	// float distance = length(tes_fragPos - dirLight.position);
	// diffuse *= 1.0 / (distance * distance);

	float shadow = calculateShadow(gs_shadowCoords);

	fragColor = vec4((ambient + shadow * diffuse), 1.0);
	fragColor = mix(vec4(skycolor, 1.0), fragColor, gs_visibility);

	if (enableWireframe)
	{
		const vec4 WIREFRAME_COLOR = vec4(0.0);
		const float WIREFRAME_THICKNESS = 1.5;

		float wireframeStrength = 0.0;

		float minDistToEdge = min(gs_distToEdges.x, min(gs_distToEdges.y, gs_distToEdges.z));
		if (minDistToEdge <= WIREFRAME_THICKNESS) {
			wireframeStrength = smoothstep(0.0, 1.0, 1.0 - (minDistToEdge / WIREFRAME_THICKNESS));
		}

		fragColor = WIREFRAME_COLOR * wireframeStrength + (1.0 - wireframeStrength) * fragColor;
	}
}

float calculateShadow(vec3 shadowCoords)
{
	const float bias = 0.005;
	
	float shadow = 1.0;
	if (texture(shadowMap, shadowCoords.xy).z < shadowCoords.z - bias)
		shadow = 0.5;

	return shadow;
}

/*
vec3 getNormalFromMap()
{
	vec3 tangentNormal = texture(normalMap, fs_in.TexCoords).xyz * 2.0 - 1.0;
	vec3 Q1 = dFdx(fs_in.FragPos);
	vec3 Q2 = dFdy(fs_in.FragPos);
	vec2 st1 = dFdx(fs_in.TexCoords);
	vec2 st2 = dFdy(fs_in.TexCoords);
	vec3 N = normalize(fs_in.Normal);
	vec3 T = normalize(Q1*st2.t - Q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);
	return normalize(TBN * tangentNormal);
}
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;
	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	return nom / max(denom, 0.001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
*/
)glsl";

constexpr const char TERRAIN_DEPTH_TES[] = R"glsl(
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
)glsl";
constexpr const char TERRAIN_DEPTH_FS[] = R"glsl(
#version 430 core

out vec4 fragColors;

void main(void)
{
	fragColors = vec4(vec3(0.0), 1.0);
}
)glsl";


constexpr const char TERRAIN2_TES[] = R"glsl(
#version 430 core

layout(quads, fractional_even_spacing) in;

uniform mat4 view;
uniform mat4 project;
uniform mat4 sunMVPMatrix;

in vec2 tcs_texCoords[];
in vec2 tcs_tileCoords[];

out vec2 tes_texCoords;
out vec2 tes_tileCoords;
out vec3 tes_fragPos;
out vec3 tes_shadowCoords;

out float visibility;

uniform sampler2D terrainMap;
uniform float terrainMaxHeight;
uniform vec4 clipPlane;

const float density = 0.00025;
uniform float gradient = 2.2;

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

	tes_tileCoords = interpolate2(tcs_tileCoords[0], tcs_tileCoords[1], tcs_tileCoords[2], tcs_tileCoords[3]);

	gl_Position = interpolate4(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);
	gl_Position.y = height * terrainMaxHeight;

	gl_ClipDistance[0] = dot(gl_Position, clipPlane);

	tes_fragPos = gl_Position.xyz;

	tes_shadowCoords = (sunMVPMatrix * gl_Position).xyz;

	vec4 positionRelativeToCam = view * gl_Position;
	gl_Position = project * positionRelativeToCam;

	float distance = length(positionRelativeToCam);
	visibility = exp(-pow(distance * density, gradient));
	visibility = clamp(visibility, 0.0, 1.0);
}
)glsl";

constexpr const char TERRAIN2_FS[] = R"glsl(
#version 430 core

in vec2 tes_texCoords;
in vec2 tes_tileCoords;
in vec3 tes_fragPos;
in vec3 tes_shadowCoords;

in float visibility;

out vec4 fragColor;

uniform sampler2D terrainMap;
uniform sampler2D splatMap;
uniform sampler2D dirtTexture;
uniform sampler2D rockTexture;
uniform sampler2D grassTexture;
uniform sampler2D wetDirtTexture;
uniform sampler2D shadowMap;

struct DirLight {
	vec3 direction;
	vec3 color;
};

uniform DirLight dirLight;

const vec4 skycolor = vec4(0.5, 0.5, 0.5, 1.0);

//float DistributionGGX(vec3 N, vec3 H, float roughness);
//float GeometrySchlickGGX(float NdotV, float roughness);
//float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
//vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

float calculateShadow(vec3 shadowCoords);

void main(void)
{
	vec4 terrain = texture(terrainMap, tes_texCoords);

	float height = terrain.w;
	vec3 normal = terrain.xzy;
	normal.xz = normal.xz * 2.0 - 1.0;

	normal = normalize(normal);

	vec4 mixmap = texture(splatMap, tes_texCoords);

	vec3 dirt = texture(dirtTexture, tes_tileCoords).rgb;
	vec3 rock = texture(rockTexture, tes_tileCoords).rgb;
	vec3 grass = texture(grassTexture, tes_tileCoords).rgb;
	vec3 wetDirt = texture(wetDirtTexture, tes_tileCoords).rgb;

	vec3 finalColor = mix(dirt, grass, mixmap.g);
	finalColor = mix(finalColor, wetDirt, mixmap.b);
	finalColor = mix(finalColor, rock, mixmap.r);
	vec3 ambient = 0.005 * finalColor;

	vec3 lightDir = normalize(dirLight.direction);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = dirLight.color * diff * finalColor;
	
	// attenuation (use quadratic as we have gamma correction)
	// float distance = length(tes_fragPos - dirLight.position);
	// diffuse *= 1.0 / (distance * distance);

	float shadow = calculateShadow(tes_shadowCoords);

	fragColor = vec4((ambient + shadow * diffuse), 1.0);
	fragColor = mix(skycolor, fragColor, visibility);
}

float calculateShadow(vec3 shadowCoords)
{
	const float bias = 0.005;
	
	float shadow = 1.0;
	if (texture(shadowMap, shadowCoords.xy).z < shadowCoords.z - bias)
		shadow = 0.5;

	return shadow;
}

/*
vec3 getNormalFromMap()
{
	vec3 tangentNormal = texture(normalMap, fs_in.TexCoords).xyz * 2.0 - 1.0;
	vec3 Q1 = dFdx(fs_in.FragPos);
	vec3 Q2 = dFdy(fs_in.FragPos);
	vec2 st1 = dFdx(fs_in.TexCoords);
	vec2 st2 = dFdy(fs_in.TexCoords);
	vec3 N = normalize(fs_in.Normal);
	vec3 T = normalize(Q1*st2.t - Q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);
	return normalize(TBN * tangentNormal);
}
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;
	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	return nom / max(denom, 0.001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
*/
)glsl";

#endif

#endif