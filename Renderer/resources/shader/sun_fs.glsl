#version 430 core

in vec2 texCoords;
out vec4 fragColors;

uniform sampler2D effectTexture;
uniform vec3 sunColor;
void main(void)
{
	vec4 color = texture(effectTexture, texCoords);

	fragColors = vec4(color.xyz * sunColor, color.w);
}