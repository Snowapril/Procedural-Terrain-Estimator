#ifndef TERRAIN_SHADER_CODE
#define TERRAIN_SHADER_CODE

	#ifndef _DEBUG
	constexpr const char BAKETERRAINMAP_VS[] = R"glsl(
	#version 430 core
	layout(location = 0) in vec3 aPos;
	layout(location = 1) in vec2 aTexCoords;
	out vec2 TexCoords;
	uniform mat4 view;
	uniform mat4 projection;
	
	void main(void)
	{
		TexCoords = aTexCoords;
		gl_Position = projection * view * vec4(aPos, 1.0);
	}
	)glsl";
	
	constexpr const char BAKETERRAINMAP_FS[] = R"glsl(
	#version 430 core
	
	in vec2 TexCoords;
	out vec4 fragColors;
	
	uniform sampler2D heightMap;
	const float offset = 0.0025333333;	

	void main(void)
	{
		vec2 offsets[9] = vec2[](
		    vec2(-offset,  offset), // top-left
		    vec2( 0.0f,    offset), // top-center
		    vec2( offset,  offset), // top-right
		    vec2(-offset,  0.0f),   // center-left
		    vec2( 0.0f,    0.0f),   // center-center
		    vec2( offset,  0.0f),   // center-right
		    vec2(-offset, -offset), // bottom-left
		    vec2( 0.0f,   -offset), // bottom-center
		    vec2( offset, -offset)  // bottom-right    
		);

		float kernel[9] = float[](
		    1.0 / 16, 2.0 / 16, 1.0 / 16,
		    2.0 / 16, 4.0 / 16, 2.0 / 16,
		    1.0 / 16, 2.0 / 16, 1.0 / 16  
		);
		
		vec3 sampleTex[9];
		float sampleHeight[9];
		const ivec3 dx_dz = ivec3(-1, 0, 1);
		const vec2 size = vec2(2.0, 0.0);

		for(int i = 0; i < 9; i++)
		{	
			vec2 texCoords_with_offset = TexCoords.st + offsets[i];
			sampleHeight[i] = texture(heightMap, texCoords_with_offset).r;
			
			float hL = textureOffset(heightMap, texCoords_with_offset, dx_dz.xy).r;
			float hR = textureOffset(heightMap, texCoords_with_offset, dx_dz.zy).r;
			float hD = textureOffset(heightMap, texCoords_with_offset, dx_dz.yx).r;
			float hU = textureOffset(heightMap, texCoords_with_offset, dx_dz.yz).r;
		    sampleTex[i] = normalize(cross(vec3(2 * offset, hR - hL, 0), vec3(0, hU - hD, 2 * offset)));
		}
		
		float total_height = 0.0;
		vec3 total_normal = vec3(0.0);
		for(int i = 0; i < 9; i++) {
			total_normal += sampleTex[i] * kernel[i];
			total_height += sampleHeight[i] * kernel[i];
		}

		total_normal.xz = total_normal.xz * 0.5 + 0.5;
	
		fragColors = vec4(total_normal.xzy, total_height); 
	}		
	)glsl";
	
	constexpr const char TERRAIN_VS[] = R"glsl(
	#version 430 core
	
	layout(location = 0) in vec3 aPos;
	layout(location = 1) in float aTessLevel;
	
	out float vs_tessLevel;
	out vec2 vs_texCoords;
	out vec2 vs_tileCoords;
	
	uniform vec3 originPos;
	uniform vec2 terrainScale;
	
	uniform float tileSize = 32.0;
	
	
	vec2 calculateTexCoords(vec3 vertex)
	{
		return ( vec2(vertex.x, vertex.z) + originPos.xz + terrainScale / 2.0 ) / terrainScale;
	}
	
	void main(void)
	{
		vs_texCoords = calculateTexCoords(aPos);
		vs_tessLevel = aTessLevel;
		vs_tileCoords = (aPos.xz / terrainScale + 0.5) * tileSize;
	
		gl_Position = vec4(aPos, 1.0);
	}
	)glsl";
	
	constexpr const char TERRAIN_TCS[] = R"glsl(
	#version 430 core
	
	layout(vertices = 4) out;
	
	uniform mat4 view;
	uniform mat4 project;
	
	in float vs_tessLevel[];
	in vec2 vs_texCoords[];
	in vec2 vs_tileCoords[];
	
	out vec2 tcs_texCoords[];
	out vec2 tcs_tileCoords[];
	
	uniform sampler2D terrainMap;
	uniform float terrainMaxHeight;
	
	uniform float minDepth;
	uniform float maxDepth;
	
	float dlodCameraDistance(vec4 p0, vec4 p1, vec2 t0, vec2 t1)
	{
		float height = texture(terrainMap, t0).a;
		p0.y = height * terrainMaxHeight;
		height = texture(terrainMap, t1).a;
		p1.y = height * terrainMaxHeight;
	
		vec4 view0 = view * p0;
		vec4 view1 = view * p1;
	
		float d0 = clamp((abs(p0.z) - minDepth) / (maxDepth - minDepth), 0.0, 1.0);
		float d1 = clamp((abs(p1.z) - minDepth) / (maxDepth - minDepth), 0.0, 1.0);
	
		float t = mix(16, 4, (d0 + d1) * 0.5);
		highp int temp = int(t - 1.0);
	
		temp |= temp >> 1;
		temp |= temp >> 2;
		temp |= temp >> 4;
		temp |= temp >> 8;
	
		t = float(temp + 1);
	
		return t;
	}
	
	void main(void)
	{
		gl_TessLevelOuter[0] = dlodCameraDistance(gl_in[3].gl_Position, gl_in[0].gl_Position, vs_texCoords[0], vs_texCoords[0]);
		gl_TessLevelOuter[0] = max(2.0, gl_TessLevelOuter[0] / vs_tessLevel[0]);
	
		gl_TessLevelOuter[1] = dlodCameraDistance(gl_in[0].gl_Position, gl_in[1].gl_Position, vs_texCoords[0], vs_texCoords[0]);
		gl_TessLevelOuter[1] = max(2.0, gl_TessLevelOuter[1] / vs_tessLevel[1]);
	
		gl_TessLevelOuter[2] = dlodCameraDistance(gl_in[1].gl_Position, gl_in[2].gl_Position, vs_texCoords[0], vs_texCoords[0]);
		gl_TessLevelOuter[2] = max(2.0, gl_TessLevelOuter[2] / vs_tessLevel[2]);
	
		gl_TessLevelOuter[3] = dlodCameraDistance(gl_in[2].gl_Position, gl_in[3].gl_Position, vs_texCoords[0], vs_texCoords[0]);
		gl_TessLevelOuter[3] = max(2.0, gl_TessLevelOuter[3] / vs_tessLevel[3]);
	
		gl_TessLevelInner[0] = (gl_TessLevelOuter[0] + gl_TessLevelOuter[3]) * 0.5;
		gl_TessLevelInner[1] = (gl_TessLevelOuter[1] + gl_TessLevelOuter[2]) * 0.5;
	
		tcs_texCoords[gl_InvocationID] = vs_texCoords[gl_InvocationID];
		tcs_tileCoords[gl_InvocationID] = vs_tileCoords[gl_InvocationID];
		
		gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	}	
	)glsl";
	
	constexpr const char TERRAIN_TES[] = R"glsl(
	#version 430 core
	
	layout(quads, fractional_even_spacing) in;
	
	in vec2 tcs_texCoords[];
	in vec2 tcs_tileCoords[];
	
	out vec2 tes_texCoords;
	out vec2 tes_tileCoords;
	
	uniform sampler2D terrainMap;
	uniform float terrainMaxHeight;
	
	
	vec4 interpolate4(vec4 v0, vec4 v1, vec4 v2, vec4 v3)
	{
		vec4 a = mix(v0, v1, gl_TessCoord.x);
		vec4 b = mix(v2, v3, gl_TessCoord.x);
	
		return mix(a, b, gl_TessCoord.y);
	}
	
	vec2 interpolate2(vec2 v0, vec2 v1, vec2 v2, vec2 v3)
	{
		vec2 a = mix(v0, v1, gl_TessCoord.x);
		vec2 b = mix(v2, v3, gl_TessCoord.x);
	
		return mix(a, b, gl_TessCoord.y);
	}
	
	void main(void)
	{
		vec2 terrainTexCoords = interpolate2(tcs_texCoords[0], tcs_texCoords[1], tcs_texCoords[2], tcs_texCoords[3]);
		float height = texture(terrainMap, terrainTexCoords).a;
	
		tes_texCoords = terrainTexCoords;
	
		tes_tileCoords = interpolate2(tcs_tileCoords[0], tcs_tileCoords[1], tcs_tileCoords[2], tcs_tileCoords[3]);
	
		gl_Position = interpolate4(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);
		gl_Position.y = height * terrainMaxHeight;
	}
	)glsl";
	
	constexpr const char TERRAIN_GS[] = R"glsl(
	#version 430 core
	
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
	out vec3 gs_normal;
	
	const float density = 0.00016;
	uniform float gradient = 1.7;
	
	vec2 projectToViewportSpace(vec4 position, mat4 vp)
	{
		vec4 clip = vp * position;
		clip.xy /= clip.w;
		clip.xy = (clip.xy * 0.5 + 0.5) * viewportSize;
		return clip.xy;
	}
	
	void emitVertex(int i, vec3 distToEdges, vec3 normal)
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
		gs_normal = normal;
	
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
	
	
	
		vec3 normal = normalize(cross(vec3(gl_in[2].gl_Position - gl_in[0].gl_Position), 
									  vec3(gl_in[1].gl_Position - gl_in[0].gl_Position)));
	
		float doubleTriangleArea = abs((e0.x * e2.y) - (e0.y * e2.x));
	
		float h0 = doubleTriangleArea / length(e0);
		float h1 = doubleTriangleArea / length(e1);
		float h2 = doubleTriangleArea / length(e2);
	
		emitVertex(0, vec3(0, h1, 0), normal);
		emitVertex(1, vec3(0, 0, h2), normal);
		emitVertex(2, vec3(h0, 0, 0), normal);
									
		EndPrimitive();
	}	
	)glsl";
	
	constexpr const char TERRAIN_FS[] = R"glsl(
	#version 430 core
	
	
	in vec2 gs_texCoords;
	in vec2 gs_tileCoords;
	in vec3 gs_fragPos;
	in vec3 gs_shadowCoords;
	in vec3 gs_distToEdges;
	
	in float gs_visibility;
	
	out vec4 fragColor;
	
	uniform sampler2D terrainMap;
	uniform sampler2D splatMap;
	uniform sampler2D dirtTexture;
	uniform sampler2D rockTexture;
	uniform sampler2D grassTexture;
	uniform sampler2D wetDirtTexture;
	uniform sampler2D shadowMap;
	uniform bool enableWireframe;
	uniform bool enableFog;
	struct DirLight {
		vec3 direction;
		vec3 color;
	};
	
	uniform DirLight dirLight;
	
	uniform vec3 skycolor = vec3(0.5, 0.5, 0.5);
	
	float calculateShadow(vec3 shadowCoords);
	in vec3 gs_normal;
	
	void main(void)
	{
		vec4 terrain = texture(terrainMap, gs_texCoords);
	
		float height = terrain.w;
		
		vec3 normal = terrain.xzy;
		normal.xz = normal.xz * 2.0 - 1.0;
		normal = normalize(normal);
	
		vec4 mixmap = texture(splatMap, gs_texCoords);
	
		vec3 dirt = texture(dirtTexture, gs_tileCoords).rgb;
		vec3 rock = texture(rockTexture, gs_tileCoords).rgb;
		vec3 grass = texture(grassTexture, gs_tileCoords).rgb;
		vec3 wetDirt = texture(wetDirtTexture, gs_tileCoords).rgb;
	
		vec3 finalColor = mix(dirt, grass, mixmap.g);
		finalColor = mix(finalColor, wetDirt, mixmap.b);
		finalColor = mix(finalColor, rock, mixmap.r);
		vec3 ambient = 0.007 * finalColor;
	
		vec3 lightDir = normalize(dirLight.direction);
		float diff = max(dot(lightDir, normal), 0.0);
		vec3 diffuse = dirLight.color * diff * finalColor;
	
		float shadow = calculateShadow(gs_shadowCoords);
	
		fragColor = vec4((ambient + shadow * diffuse), 1.0);
		if (enableFog)
			fragColor = mix(vec4(skycolor, 1.0), fragColor, gs_visibility);
	
		if (enableWireframe)
		{
			const vec4 WIREFRAME_COLOR = vec4(0.0, 1.0, 0.0, 1.0);
			const float WIREFRAME_THICKNESS = 1.1;
	
			float wireframeStrength = 0.0;
	
			float minDistToEdge = min(gs_distToEdges.x, min(gs_distToEdges.y, gs_distToEdges.z));
			if (minDistToEdge <= WIREFRAME_THICKNESS) {
				wireframeStrength = smoothstep(0.0, 1.0, 1.0 - (minDistToEdge / WIREFRAME_THICKNESS));
			}
	
			fragColor = WIREFRAME_COLOR * wireframeStrength + (1.0 - wireframeStrength) * fragColor;
		}
	}
	
	float calculateShadow(vec3 shadowCoords)
	{
		const float bias = 0.005;
		
		float shadow = 1.0;
		if (texture(shadowMap, shadowCoords.xy).z < shadowCoords.z - bias)
			shadow = 0.5;
	
		return shadow;
	}
	)glsl";
	
	constexpr const char TERRAIN_DEPTH_TES[] = R"glsl(
	#version 430 core
	
	layout(quads, fractional_even_spacing) in;
	
	uniform mat4 sunMVPMatrix;
	
	in vec2 tcs_texCoords[];
	in vec2 tcs_tileCoords[];
	
	uniform sampler2D terrainMap;
	uniform float terrainMaxHeight;
	uniform vec4 clipPlane;
	
	uniform mat4 view;
	uniform mat4 project;
	
	uniform bool enableVP;
	
	vec4 interpolate4(vec4 v0, vec4 v1, vec4 v2, vec4 v3)
	{
		vec4 a = mix(v0, v1, gl_TessCoord.x);
		vec4 b = mix(v2, v3, gl_TessCoord.x);
	
		return mix(a, b, gl_TessCoord.y);
	}
	
	vec2 interpolate2(vec2 v0, vec2 v1, vec2 v2, vec2 v3)
	{
		vec2 a = mix(v0, v1, gl_TessCoord.x);
		vec2 b = mix(v2, v3, gl_TessCoord.x);
	
		return mix(a, b, gl_TessCoord.y);
	}
	
	void main(void)
	{
		vec2 terrainTexCoords = interpolate2(tcs_texCoords[0], tcs_texCoords[1], tcs_texCoords[2], tcs_texCoords[3]);
		float height = texture(terrainMap, terrainTexCoords).a;
	
		gl_Position = interpolate4(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);
		gl_Position.y = height * terrainMaxHeight;
	
		gl_ClipDistance[0] = dot(gl_Position, clipPlane);
	
		if (enableVP)
		{
			gl_Position = project * view * gl_Position;
		}
		else
		{
			gl_Position = sunMVPMatrix * gl_Position;
		}
	}
	)glsl";
	constexpr const char TERRAIN_DEPTH_FS[] = R"glsl(
	#version 430 core
	
	out vec4 fragColors;
	
	void main(void)
	{
		fragColors = vec4(vec3(0.0), 1.0);
	}
	)glsl";
	
	
	constexpr const char TERRAIN2_TES[] = R"glsl(
	#version 430 core
	
	layout(quads, fractional_even_spacing) in;
	
	uniform mat4 view;
	uniform mat4 project;
	uniform mat4 sunMVPMatrix;
	
	in vec2 tcs_texCoords[];
	in vec2 tcs_tileCoords[];
	
	out vec2 tes_texCoords;
	out vec2 tes_tileCoords;
	out vec3 tes_fragPos;
	out vec3 tes_shadowCoords;
	
	out float visibility;
	
	uniform sampler2D terrainMap;
	uniform float terrainMaxHeight;
	uniform vec4 clipPlane;
	
	const float density = 0.00025;
	uniform float gradient = 2.2;
	
	vec4 interpolate4(vec4 v0, vec4 v1, vec4 v2, vec4 v3)
	{
		vec4 a = mix(v0, v1, gl_TessCoord.x);
		vec4 b = mix(v2, v3, gl_TessCoord.x);
	
		return mix(a, b, gl_TessCoord.y);
	}
	
	vec2 interpolate2(vec2 v0, vec2 v1, vec2 v2, vec2 v3)
	{
		vec2 a = mix(v0, v1, gl_TessCoord.x);
		vec2 b = mix(v2, v3, gl_TessCoord.x);
	
		return mix(a, b, gl_TessCoord.y);
	}
	
	void main(void)
	{
		vec2 terrainTexCoords = interpolate2(tcs_texCoords[0], tcs_texCoords[1], tcs_texCoords[2], tcs_texCoords[3]);
		float height = texture(terrainMap, terrainTexCoords).a;
	
		tes_texCoords = terrainTexCoords;
	
		tes_tileCoords = interpolate2(tcs_tileCoords[0], tcs_tileCoords[1], tcs_tileCoords[2], tcs_tileCoords[3]);
	
		gl_Position = interpolate4(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);
		gl_Position.y = height * terrainMaxHeight;
	
		gl_ClipDistance[0] = dot(gl_Position, clipPlane);
	
		tes_fragPos = gl_Position.xyz;
	
		tes_shadowCoords = (sunMVPMatrix * gl_Position).xyz;
	
		vec4 positionRelativeToCam = view * gl_Position;
		gl_Position = project * positionRelativeToCam;
	
		float distance = length(positionRelativeToCam);
		visibility = exp(-pow(distance * density, gradient));
		visibility = clamp(visibility, 0.0, 1.0);
	}
	)glsl";
	
	constexpr const char TERRAIN2_FS[] = R"glsl(
	#version 430 core
	
	in vec2 tes_texCoords;
	in vec2 tes_tileCoords;
	in vec3 tes_fragPos;
	in vec3 tes_shadowCoords;
	
	in float visibility;
	
	out vec4 fragColor;
	
	uniform sampler2D terrainMap;
	uniform sampler2D splatMap;
	uniform sampler2D dirtTexture;
	uniform sampler2D rockTexture;
	uniform sampler2D grassTexture;
	uniform sampler2D wetDirtTexture;
	uniform sampler2D shadowMap;
	
	struct DirLight {
		vec3 direction;
		vec3 color;
	};
	
	uniform DirLight dirLight;
	
	const vec4 skycolor = vec4(0.5, 0.5, 0.5, 1.0);
	
	float calculateShadow(vec3 shadowCoords);
	
	void main(void)
	{
		vec4 terrain = texture(terrainMap, tes_texCoords);
	
		float height = terrain.w;
		vec3 normal = terrain.xzy;
		normal.xz = normal.xz * 2.0 - 1.0;
	
		normal = normalize(normal);
	
		vec4 mixmap = texture(splatMap, tes_texCoords);
	
		vec3 dirt = texture(dirtTexture, tes_tileCoords).rgb;
		vec3 rock = texture(rockTexture, tes_tileCoords).rgb;
		vec3 grass = texture(grassTexture, tes_tileCoords).rgb;
		vec3 wetDirt = texture(wetDirtTexture, tes_tileCoords).rgb;
	
		vec3 finalColor = mix(dirt, grass, mixmap.g);
		finalColor = mix(finalColor, wetDirt, mixmap.b);
		finalColor = mix(finalColor, rock, mixmap.r);
		vec3 ambient = 0.005 * finalColor;
	
		vec3 lightDir = normalize(dirLight.direction);
		float diff = max(dot(lightDir, normal), 0.0);
		vec3 diffuse = dirLight.color * diff * finalColor;
		
		float shadow = calculateShadow(tes_shadowCoords);
	
		fragColor = vec4((ambient + shadow * diffuse), 1.0);
		fragColor = mix(skycolor, fragColor, visibility);
	}
	
	float calculateShadow(vec3 shadowCoords)
	{
		const float bias = 0.005;
		
		float shadow = 1.0;
		if (texture(shadowMap, shadowCoords.xy).z < shadowCoords.z - bias)
			shadow = 0.5;
	
		return shadow;
	}
	
	)glsl";
	#endif

#endif