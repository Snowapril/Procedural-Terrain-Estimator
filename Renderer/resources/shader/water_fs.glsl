#version 430 core

in vec4 clipPosition;
out vec4 fragColors;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;

void main(void)
{
	vec2 texCoords = clipPosition.xy / clipPosition.w;
	texCoords = texCoords * 0.5 + 0.5;

	vec2 reflectionTexCoords = vec2(texCoords.x, -texCoords.y);
	vec2 refractionTexCoords = vec2(texCoords.x, texCoords.y);

	vec3 reflection = texture(reflectionTexture, reflectionTexCoords).rgb;
	vec3 refraction = texture(refractionTexture, refractionTexCoords).rgb;

	vec3 finalColor = mix(reflection, refraction, 0.5);

    fragColors = vec4(finalColor, 1.0);
}