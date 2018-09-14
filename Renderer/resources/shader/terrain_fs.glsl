#version 430 core

in vec2 tes_texCoords;
in vec2 tes_tessCoords;
in vec2 tes_tileCoords;

out vec4 fragColor;

uniform float terrainMaxHeight;
uniform float terrainHeightOffset;

uniform sampler2D terrainMap;

uniform sampler2D splatMap;
uniform sampler2D dirtTexture;
uniform sampler2D rockTexture;
uniform sampler2D grassTexture;

uniform vec3 wireColor;

void main(void)
{
	vec4 terrain = texture(terrainMap, tes_texCoords);
	
	float height = terrain.w;
	vec3 normal = normalize(terrain.xyz);

	vec4 mixmap = texture(splatMap, tes_texCoords);
	
	vec3 dirt = texture(dirtTexture, tes_tileCoords).rgb;
	vec3 rock = texture(rockTexture, tes_tileCoords).rgb;
	vec3 water = texture(grassTexture, tes_tileCoords).rgb;
	
	vec3 red = vec3(1.0, 0.0, 0.0);
	vec3 blue = vec3(0.0, 0.0, 1.0);
	vec3 mixColor = mix(blue, red, height);

	vec3 finalColor = mix(dirt, water, mixmap.b);
	finalColor = mix(finalColor, rock, mixmap.r) * 0.0 + mixColor;

	vec3 lightDir = normalize(vec3(1.f, 1.f, 0.f));
	vec3 lightDiffuse = vec3(0.9);
	float diff = max(dot(lightDir, normal), 0.0);
	
	vec3 diffuse = diff * lightDiffuse;
	vec3 ambient = vec3(0.35);

	finalColor = (ambient + diffuse) * finalColor;

	fragColor = vec4(finalColor * wireColor, 1.0);
}