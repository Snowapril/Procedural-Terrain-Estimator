#version 430 core

in vec4 clipPosition;
in vec2 texCoords;
in vec3 toCameraVector;
in vec3 fragPos;
//in float visibility;
out vec4 fragColors;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float moveFactor;

struct DirLight {
	vec3 position;
	vec3 color;
};

uniform DirLight dirLight;

const float distortionStrength	= 0.05;
const float shineDamper			= 20.0;
const float reflectivity		= 0.6;

//const vec4 skycolor = vec4(0.5, 0.5, 0.5, 1.0);

void main(void)
{
	vec2 ndc = (clipPosition.xy / clipPosition.w) / 2.0 + 0.5f;

	vec2 reflectionTexCoords = vec2(ndc.x, -ndc.y);
	vec2 refractionTexCoords = vec2(ndc.x, ndc.y);
	
	const float near = 3.0;
	const float far = 1000.0;
	float depth = texture(depthMap, refractionTexCoords).r;
	float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
	
	depth = gl_FragCoord.z;
	float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
	
	float waterDepth = floorDistance - waterDistance;
	
	vec2 distortedTexCoords = texture(dudvMap, vec2(texCoords.x + moveFactor, texCoords.y)).rg * 0.1;
	distortedTexCoords = texCoords + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
	vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * distortionStrength * clamp(waterDepth / 20.0, 0.0, 1.0);
	
	vec4 normalColor = texture(normalMap, distortedTexCoords);
	vec3 normal = normalize(vec3(normalColor.r * 2.0 - 1.0, normalColor.b * 3.0, normalColor.g * 2.0 - 1.0));
	
	
	reflectionTexCoords += totalDistortion;
	reflectionTexCoords.x = clamp(reflectionTexCoords.x, 0.001, 0.999);
	reflectionTexCoords.y = clamp(reflectionTexCoords.y, -0.999, -0.001);
	
	refractionTexCoords += totalDistortion;
	refractionTexCoords = clamp(refractionTexCoords, 0.001, 0.999);

	vec4 reflection = texture(reflectionTexture, reflectionTexCoords);
	vec4 refraction = texture(refractionTexture, refractionTexCoords);

	vec3 viewVector = normalize(toCameraVector);
	float reflectionFactor = dot(viewVector, normal);
	reflectionFactor = pow(reflectionFactor, 3.0);

	vec3 reflectedLight = reflect(normalize(fragPos - dirLight.position), normal);
	float specular = max(dot(reflectedLight, viewVector), 0.0);
	specular = pow(specular, shineDamper);
	vec3 specularHighlight = dirLight.color * specular * reflectivity * clamp(waterDepth / 5.0, 0.0, 1.0);

	fragColors = mix(reflection, refraction, reflectionFactor);
	fragColors = mix(fragColors, vec4(0.0, 0.3, 0.5, 1.0), 0.2) + vec4(specularHighlight, 1.0);
	//fragColors = mix(skycolor, fragColors, visibility);
	fragColors.a = clamp(waterDepth / 5.0, 0.0, 1.0);
}