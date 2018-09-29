#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

out vec3 localPos;

void main(void)
{
    localPos = aPos;
    gl_Position = projection * view * vec4(localPos, 1.0);
}