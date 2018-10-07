#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 texCoords;
out mat4 inverseVP;
out vec2 viewportLightPos;

uniform vec3 lightPosition;
uniform mat4 view;
uniform mat4 project;

void main(void)
{
	texCoords = aTexCoords;

	mat4 vp = project * view;
	inverseVP = inverse(vp);

	vec4 clipLightPos = vp * vec4(lightPosition, 1.0);
	clipLightPos.xy /= clipLightPos.w;
	viewportLightPos = (clipLightPos.xy * 0.5 + 0.5);

    gl_Position = vec4(aPos, 0.0, 1.0);
}