#ifndef TERRAIN_SHADER_CODE
#define TERRAIN_SHADER_CODE

#ifndef _DEBUG

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
uniform bool depthRender;
uniform float near;
uniform float far;

in vec2 texCoords;
out vec4 fragColors;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // back to NDC 
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main(void)
{
	vec3 color;

	if (depthRender)
	{
		float depth = texture(textureView, texCoords).r;
		depth = LinearizeDepth(depth);

		color = vec3(depth);
	}
	else
	{
		color = texture(textureView, texCoords).rgb;
	}

	fragColors = vec4(color, 1.0);
}
)glsl";


#endif

#endif