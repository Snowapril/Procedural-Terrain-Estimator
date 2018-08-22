#version 430 core

in vec2 TexCoords;
out vec4 fragColors;

uniform sampler2D heightMap;
uniform sampler2D normalMap;

void main(void)
{
    fragColors = vec4(texture(normalMap, TexCoords).rgb,
                        texture(heightMap, TexCorods).r);
}