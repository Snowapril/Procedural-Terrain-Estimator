#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in uint aTessLevel;

out uint vs_tessLevel;

void main(void)
{
	vs_tessLevel = aTessLevel;

	gl_Position = vec4(aPos, 1.0);
}