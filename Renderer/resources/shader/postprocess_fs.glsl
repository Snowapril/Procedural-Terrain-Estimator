#version 430 core

in vec2 texCoords;
out vec4 fragColors;

uniform sampler2D hdrBuffer;
uniform sampler2D depthTexture;

uniform mat4 view;
uniform mat4 project;
uniform mat4 previousVP;

const float blurStrength = 0.06f;
const int numSamples = 7;
const float GAMMA = 2.2;

void main(void)
{
	vec2 localTexCoords = texCoords;

	float depth = texture(depthTexture, localTexCoords).r;

	vec4 currentPos = vec4(localTexCoords.x * 2.0 - 1.0, localTexCoords.y * 2.0 - 1, depth, 1);

	mat4 inverseVP = inverse(project * view);

	vec4 D = inverseVP * currentPos;
	vec4 worldPos = D / D.w;

	vec4 previousPos = previousVP * worldPos;
	previousPos /= previousPos.w;

	vec2 velocity = ((currentPos - previousPos) * blurStrength).xy;
		
	vec3 hdrColor = texture(hdrBuffer, localTexCoords).rgb;

	localTexCoords += velocity;

	for (int i = 1; i < numSamples; ++i, localTexCoords += velocity)
	{
		localTexCoords = clamp(localTexCoords, 0.0, 1.0);
		vec3 currentColor = texture(hdrBuffer, localTexCoords).rgb;
		hdrColor += currentColor;
	}

	hdrColor /= numSamples;

	vec3 result = hdrColor / (hdrColor + vec3(1.0));
	result = pow(hdrColor, vec3(1.0 / GAMMA));
	fragColors = vec4(result, 1.0);
}