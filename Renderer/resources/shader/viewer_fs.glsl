#version 430 core

uniform sampler2D textureView;
in vec2 texCoords;
out vec4 fragColors;

void main(void)
{
    fragColors = texture(textureView, texCoords);    
}