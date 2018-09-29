#ifndef TERRAIN_SHADER_CODE
#define TERRAIN_SHADER_CODE

#ifdef _DEBUG

constexpr const char VIEWER_VS[] = R"glsl(
#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 model;

out vec2 texCoords;

void main(void)
{
    texCoords = aTexCoords;

    gl_Position = model * vec4(aPos, 0.0, 1.0);
}
)glsl";

constexpr const char VIEWER_FS[] = R"glsl(
#version 430 core

uniform sampler2D textureView;
in vec2 texCoords;
out vec4 fragColors;

void main(void)
{
    fragColors = texture(textureView, texCoords);    
}
)glsl";


#endif

#endif