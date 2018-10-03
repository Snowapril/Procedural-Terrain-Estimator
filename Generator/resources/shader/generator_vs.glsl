#version 430 core

layout (location = 0) in vec2 aPos;

out vec2 noisePos;

void main(void)
{
    noisePos = aPos;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}