#ifndef TERRAIN_SHADER_CODE
#define TERRAIN_SHADER_CODE

#ifndef _DEBUG

constexpr const char SKYBOX_VS[] = R"glsl(
#version 430 core

layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 project;

out vec3 texCoords;

void main(void)
{
    texCoords = aPos;
    vec4 position = project * view * vec4(aPos, 1.0);
    gl_Position = position.xyww;
}
)glsl";

constexpr const char SKYBOX_FS[] = R"glsl(
#version 430 core

in vec3 texCoords;
out vec4 fragColor;

uniform samplerCube cubeMap;

uniform vec3 skycolor = vec3(0.5, 0.5, 0.5);

uniform float lowerLimit = 0.0;
uniform float upperLimit = 0.1;

void main(void)
{
	vec4 envColor = texture(cubeMap, texCoords);

	float factor = (texCoords.y - lowerLimit) / (upperLimit - lowerLimit);

	factor = clamp(factor, 0.0, 1.0);
	
	fragColor = mix(vec4(skycolor, 1.0), envColor, factor);
}
)glsl";


constexpr const char EQUIRECTMAPTOCUBEMAP_VS[] = R"glsl(
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
)glsl";


constexpr const char EQUIRECTMAPTOCUBEMAP_FS[] = R"glsl(
#version 430 core

in vec3 localPos;
out vec4 fragColor;

uniform sampler2D equiRectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;

    return uv;
}

void main(void)
{
    vec2 uv = SampleSphericalMap(normalize(localPos));
    vec3 color = texture(equiRectangularMap, uv).rgb;

    fragColor = vec4(color, 1.0);
}
)glsl";

#endif

#endif