#version 430 core

layout (location = 0) in vec2 aPos;

uniform mat4 project;

out vec2 noisePos;

void main(void)
{
    gl_Position = project * vec4(aPos.x, aPos.y, 0.0, 1.0);
	noisePos = vec2(aPos.x, aPos.y);
}