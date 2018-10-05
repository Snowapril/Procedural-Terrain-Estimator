#version 430 core

/*
reference :
http://developer.download.nvidia.com/SDK/10/direct3d/Source/SolidWireframe/Doc/SolidWireframe.pdf
*/

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 view;
uniform mat4 project;
uniform mat4 sunMVPMatrix;
uniform vec4 clipPlane;
uniform vec2 viewportSize;

in vec2 tes_texCoords[];
in vec2 tes_tileCoords[];

out vec3 gs_fragPos;
out vec3 gs_shadowCoords;
out float gs_visibility;
out vec2 gs_texCoords;
out vec2 gs_tileCoords;
out vec3 gs_distToEdges;

const float density = 0.00025;
const float gradient = 2.2;

vec2 projectToViewportSpace(vec4 position, mat4 vp)
{
	vec4 clip = vp * position;
	clip.xy /= clip.w;
	// -1.0 ~ 1.0 -> 0.0 ~ 1.0 -> width ~ height
	clip.xy = (clip.xy * 0.5 + 0.5) * viewportSize;
	return clip.xy;
}

void emitVertex(int i, vec3 distToEdges)
{
	gs_fragPos = gl_in[i].gl_Position.xyz;
	gs_shadowCoords = (sunMVPMatrix * gl_in[i].gl_Position).xyz;

	vec4 positionRelativeToCam = view * gl_in[i].gl_Position;

	float distance = length(positionRelativeToCam);
	gs_visibility = exp(-pow(distance * density, gradient));
	gs_visibility = clamp(gs_visibility, 0.0, 1.0);

	gs_texCoords = tes_texCoords[i];
	gs_tileCoords = tes_tileCoords[i];
	gs_distToEdges = distToEdges;

	gl_ClipDistance[0] = dot(gl_in[i].gl_Position, clipPlane);
	gl_Position = project * positionRelativeToCam;

	EmitVertex();
}

void main(void)
{
	mat4 vp = project * view;
	
	vec2 p0 = projectToViewportSpace(gl_in[0].gl_Position, vp);
	vec2 p1 = projectToViewportSpace(gl_in[1].gl_Position, vp);
	vec2 p2 = projectToViewportSpace(gl_in[2].gl_Position, vp);

	vec2 e0 = p1 - p0;
	vec2 e1 = p2 - p1;
	vec2 e2 = p2 - p0;

	// cross((b-a), (c-a)).
	// cross( (e1 - e0), (e2 - e0) );
	// ( e1.x - e0.x, e1.y - e0.y, e1.z - e0.z) x (e2.x - e0.x, e2.y - e0.y, e2.z - e0.z)
	// e1.y * e2.z - e1.y * e0.z - e0.y * e2.z + e0.z * e0.y -  .....

	float doubleTriangleArea = abs((e0.x * e2.y) - (e0.y * e2.x));

	float h0 = doubleTriangleArea / length(e0);
	float h1 = doubleTriangleArea / length(e1);
	float h2 = doubleTriangleArea / length(e2);

	emitVertex(0, vec3(0, h1, 0));
	emitVertex(1, vec3(0, 0, h2));
	emitVertex(2, vec3(h0, 0, 0));

	EndPrimitive();
}	