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