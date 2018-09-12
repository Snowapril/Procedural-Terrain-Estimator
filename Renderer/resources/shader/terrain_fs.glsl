#version 430 core

//uniform sampler2D terrainMap;
//uniform sampler2D splatMap;
//uniform sampler2D grassTexture;
//uniform sampler2D dirtTexture;

in vec2 tes_texCoords;
//in vec2 tes_tessCoords;

out vec4 fragColor;

uniform sampler2D terrainMap;

uniform vec3 wireColor;

void main(void)
{
	//vec3 grass = texture(grassTexture, tes_tessCoords).rgb;
	//vec3 dirt = texture(dirtTexture, tes_tessCoords).rgb;
	//
	//vec3 color = mix(grass, dirt, texture(splatMap, tes_texCoords).g);

	//vec4 terrain = texture(terrainMap, tes_texCoords);
	float height = texture(terrainMap, tes_texCoords).r;

	//vec3 normal = terrain.rgb;

	vec3 blue = vec3(0.0f, 0.0f, 0.9f);
	vec3 red = vec3(0.9f, 0.35f, 0.0f);
	
	vec3 color = mix(blue, red, height);// +normal * 0.0;

	fragColor = vec4(color * wireColor, 1.0);
}