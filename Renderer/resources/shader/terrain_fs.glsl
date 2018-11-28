#version 430 core


in vec2 gs_texCoords;
in vec2 gs_tileCoords;
in vec3 gs_fragPos;
in vec3 gs_shadowCoords;
in vec3 gs_distToEdges;
in vec3 gs_normal;
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
uniform bool enableTriangleNormal;

struct DirLight {
	vec3 direction;
	vec3 color;
};

uniform DirLight dirLight;

uniform vec3 skycolor = vec3(0.5, 0.5, 0.5);

float calculateShadow(vec3 shadowCoords);

void main(void)
{
	vec4 terrain = texture(terrainMap, gs_texCoords);

	float height = terrain.w;

	vec3 normal;
	if (enableTriangleNormal) {
		normal = gs_normal;
	}
	else {
		normal = terrain.xzy;
		normal.xz = normal.xz * 2.0 - 1.0;
	}
	normal = normalize(normal);

	vec4 mixmap = texture(splatMap, gs_texCoords);

	vec3 dirt = texture(dirtTexture, gs_tileCoords).rgb;
	vec3 rock = texture(rockTexture, gs_tileCoords).rgb;
	vec3 grass = texture(grassTexture, gs_tileCoords).rgb;
	vec3 wetDirt = texture(wetDirtTexture, gs_tileCoords).rgb;

	vec3 finalColor = mix(dirt, grass, mixmap.g);
	finalColor = mix(finalColor, wetDirt, mixmap.b);
	finalColor = mix(finalColor, rock, mixmap.r);
	vec3 ambient = 0.05 * finalColor;

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
		const float WIREFRAME_THICKNESS = 1.3;

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
		shadow = 0.6;

	return shadow;
}