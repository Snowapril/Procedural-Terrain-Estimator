#version 430 core

layout (location = 0) in vec3 aPos;

layout (std140) uniform VP {
    mat4 view;
    mat4 project;
};

out vec3 texCoords;

void main(void)
{
    texCoords = aPos;
    mat4 view_no_translate = mat4(mat3(view));
    vec4 position = project * view_no_translate * vec4(aPos, 1.0);
    gl_Position = position.xyww;
}