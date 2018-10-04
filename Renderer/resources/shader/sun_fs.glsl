#version 430 core

in vec2 texCoords;
out vec4 fragColors;

uniform sampler2D effectTexture;

void main(void)
{
	fragColors = texture(effectTexture, texCoords);
}