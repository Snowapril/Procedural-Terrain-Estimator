#version 430 core

in vec2 texCoords;
out vec4 fragColors;

uniform sampler2D effectTexture;
uniform float brightness;

void main(void)
{
	vec4 finalColor = texture(effectTexture, texCoords);

	fragColors = vec4(finalColor.xyz * brightness, finalColor.w);
}