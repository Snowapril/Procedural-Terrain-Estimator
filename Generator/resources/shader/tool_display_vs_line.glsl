#version 430 core
layout (location = 0) in vec2 aPos;

uniform mat4 model;

void main(void) 
{
	vec2 position = vec2(model * vec4(aPos.x, aPos.y, 0.0, 1.0));
	position = vec2(2.0, 2.0) * (position - vec2(0.5, 0.5));
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}