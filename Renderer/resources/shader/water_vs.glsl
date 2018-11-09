#version 430 core

layout (location = 0) in vec2 aPos;

uniform vec3 viewPos;

out vec4 clipPosition;
out vec2 texCoords;
out vec3 toCameraVector;
out vec3 fragPos;
//out float visibility;

uniform vec3 lightPosition;

uniform mat4 view;
uniform mat4 project;

uniform mat4 model;

uniform float tiling = 6.0;
//const float density = 0.0001;
//const float gradient = 2.4;

void main(void) 
{
	vec4 worldPosition = model * vec4(aPos.x, 0.0, aPos.y, 1.0);
	vec4 positionRelativeToCam = view * worldPosition;
	clipPosition = project * positionRelativeToCam;
	gl_Position = clipPosition;

	texCoords = (aPos * 0.5 + 0.5) * tiling;
	toCameraVector = viewPos - worldPosition.xyz;
	fragPos = worldPosition.xyz;

	//float distance = length(positionRelativeToCam);
	//visibility = exp(-pow(distance * density, gradient));
	//visibility = clamp(visibility, 0.0, 1.0);
}