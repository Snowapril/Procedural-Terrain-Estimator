#ifndef TERRAIN_SHADER_CODE
#define TERRAIN_SHADER_CODE

#ifdef _DEBUG

constexpr const char SKYBOX_VS[] = R"glsl(
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
)glsl";

constexpr const char SKYBOX_FS[] = R"glsl(
#version 430 core

in vec3 texCoords;
out vec4 fragColor;

uniform samplerCube cubeMap;

const float GAMMA = 2.2;

void main(void)
{
	vec3 envColor = texture(cubeMap, texCoords).rgb;

	envColor = envColor / (envColor + vec3(1.0));
	envColor = pow(envColor, vec3(1.0 / GAMMA));

	fragColor = vec4(envColor, 1.0);
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