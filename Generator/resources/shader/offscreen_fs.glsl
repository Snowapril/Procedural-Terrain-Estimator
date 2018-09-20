#version 430 core

uniform sampler2D framebufferTexture;

in vec2 texCoords;
out vec4 fragColors;

void main(void) 
{
    fragColors = texture(framebufferTexture, texCoords);
}