#version 430 core

layout(vertices = 4) out;

in VS_OUT {
	vec2 terrainTexCoords;
} tcs_in;

void main(void)
{
    TexCoords = aTexCoords;

    gl_Position = project * view * model * vec4(aPos, 1.0);
}