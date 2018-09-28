#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec2 tes_texCoords[];

out vec3 gs_normal;
out vec2 gs_texCoords;

void main(void)
{
	vec3 a = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 b = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 resultN = normalize(cross(b, a));
	
	mat4 vp = project * view;

	for (int i = 0; i < 3; i++)
	{
		gl_Position = vp * gl_in[i].gl_Position;
		gs_normal = resultN;
		gs_texCoords = tes_texCoords[i];
		EmitVertex();
	}
}	