#ifndef POSTPROCESS_SHADER_CODE
#define POSTPROCESS_SHADER_CODE

#ifndef _DEBUG

constexpr const char POSTPROCESS_VS[] = R"glsl(
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
)glsl";

constexpr const char POSTPROCESS_FS[] = R"glsl(
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
uniform int godRaysSamples = 150;
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
)glsl";

constexpr const char SUN_VS[] = R"glsl(
#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 texCoords;

uniform mat4 view;
uniform mat4 project;
uniform mat4 model;

void main(void)
{
	texCoords = aTexCoords;

    vec4 position = project * view * model * vec4(aPos, 0.0, 1.0);
	gl_Position = position.xyww;
}
)glsl";

constexpr const char SUN_FS[] = R"glsl(
#version 430 core

in vec2 texCoords;
out vec4 fragColors;

uniform sampler2D effectTexture;
uniform vec3 sunColor;
void main(void)
{
	vec4 color = texture(effectTexture, texCoords);

	fragColors = vec4(color.xyz * sunColor, color.w);
}
)glsl";

#endif

#endif