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

uniform vec3 wireColor;

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

	fragColor = vec4(shadow * (ambient + diffuse) * wireColor, 1.0);
	fragColor = mix(skycolor, fragColor, visibility);
}

float calculateShadow(vec3 shadowCoords)
{
	float shadow = 1.0;

	if (texture(shadowMap, shadowCoords.xy).z < shadowCoords.z)
		shadow = 0.1f;

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