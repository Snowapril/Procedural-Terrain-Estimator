#version 430 core

in vec2 TexCoords;
out vec4 fragColors;

void main(void)
{
    fragColors = vec4(vec3((TexCoords.x + TexCoords.y) / 2.0), 1.0);
}	