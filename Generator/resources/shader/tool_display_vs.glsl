#version 430 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 model;

out vec2 texCoords;

void main(void) 
{
	texCoords = aTexCoords;
    gl_Position = model * vec4(aPos.x, aPos.y, 0.0, 1.0);
}