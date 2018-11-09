#version 430 core

in vec2 texCoords;
in mat4 inverseVP;
in vec2 viewportLightPos;

out vec4 fragColors;

uniform sampler2D hdrBuffer;
uniform sampler2D depthTexture;
uniform sampler2D occludeTexture;

uniform mat4 view;
uniform mat4 project;
uniform mat4 previousVP;

//motion blur
uniform float blurStrength = 0.06f;
uniform int numSamples = 7;
uniform float GAMMA = 2.2;

// god rays
uniform float decay = 0.96875;
uniform int godRaysSamples = 100;
uniform float exposure = 0.5;
uniform float weight = 0.5;

vec3 motionBlur(vec2 localTexCoords, float depth);
vec3 godrays(vec2 localTexCoords);
vec3 occludeLight(vec2 localTexCoords);

void main(void)
{
	float depth = texture(depthTexture, texCoords).r;

	vec3 motionBlurredColor = motionBlur(texCoords, depth);
	vec3 godRayResult = godrays(texCoords);

	vec3 combination = motionBlurredColor + godRayResult;

	vec3 result = combination / (combination + vec3(1.0));
	result = pow(combination, vec3(1.0 / GAMMA));
	fragColors = vec4(result, 1.0);
}

vec3 motionBlur(vec2 localTexCoords, float depth)
{
	vec4 currentPos = vec4(localTexCoords.x * 2.0 - 1.0, localTexCoords.y * 2.0 - 1, depth, 1);

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

	return hdrColor;
}

vec3 godrays(vec2 localTexCoords)
{
	vec2 duv = localTexCoords - viewportLightPos;
	duv /= godRaysSamples;

	vec3 color = texture(occludeTexture, localTexCoords).xyz; //occludeLight(localTexCoords);
	float illumDec = 1.0;

	vec2 suv = localTexCoords;

	for (int i = 0; i < godRaysSamples; ++i)
	{
		suv -= duv;
		vec3 samp = texture(occludeTexture, suv).xyz; //occludeLight(suv);

		samp *= illumDec * weight;
		color += samp;
		illumDec *= decay;
	}
	
	return color * exposure;
}

vec3 occludeLight(vec2 localTexCoords)
{
	float depth = texture(depthTexture, localTexCoords).r;
	depth = depth == 1.0 ? 1.0 : 0.0;

	return vec3(depth);
}