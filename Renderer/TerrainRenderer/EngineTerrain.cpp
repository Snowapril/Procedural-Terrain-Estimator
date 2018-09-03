#include "EngineTerrain.hpp"
#include "GLShader.hpp"
#include "GLResources.hpp"
#include "AssetManager.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "EngineLogger.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

EngineTerrain::EngineTerrain()
	: terrainMap(0), heightData(nullptr), prevCameraPos(0.0f)
{
}

EngineTerrain::~EngineTerrain()
{
	glDeleteTextures(1, &terrainMap);

	if (heightData)
		stbi_image_free(heightData);
}

EngineTerrain::EngineTerrain(const EngineTerrain & other)
	: terrainMap(other.terrainMap), prevCameraPos(other.prevCameraPos)
{
	//assetManager.reset(new AssetManager(*other.assetManager));
	//terrainShader = assetManager->addAsset()

	if (heightData)
		stbi_image_free(heightData);

	//deep copy
}

EngineTerrain & EngineTerrain::operator=(const EngineTerrain & other)
{
	if (&other == this)
		return *this;

	terrainMap = other.terrainMap;

	if (heightData)
		stbi_image_free(heightData);

	prevCameraPos = other.prevCameraPos;

	//assetManager.reset(new AssetManager(*other.assetManager));
	//terrainShader

	return *this;
}


void EngineTerrain::buildNonUniformPatch(const glm::vec3 & cameraPos) noexcept
{
	if (cameraPos == prevCameraPos)
	{
		prevCameraPos = cameraPos;
		return;
	}

	/// build non-uniform patch (with quad tree) here.
}

/**
* @ brief		draw Terrain with tessellation (LODing technique)
*/
void EngineTerrain::drawTerrain(unsigned int drawMode) const
{
	terrainShader->useProgram();
	//draw
}

/**
* @ brief		init context for rendering terrain with local files(height map, normal map, etc ..)
* @ details		initialize resources which is needed for rendering terrain with local files pre-compared.
				advantage of this approach is 'speed', bacause height map and normal map, etc .. are precompared, thus 
				no need to generate in run-time.
				disadvantage of this approach is "will generate fixed result".
* @ return		return boolean whether if initialization is successful or not.
*/
bool EngineTerrain::initWithLocalFile(float aspectRatio, std::initializer_list<std::string>&& paths)
{
	assetManager = std::make_unique<AssetManager>();
	try
	{
		terrainShader = assetManager->addAsset<GLShader>({
			"../resources/shader/terrain_vs.glsl",
			"../resources/shader/terrain_tcs.glsl",
			"../resources/shader/terrain_tes.glsl",
			"../resources/shader/terrain_fs.glsl"
		});
	}
	catch (std::exception e)
	{
		EngineLogger::getConsole()->error("Failed to init EngineTerrain (cannot open shader)");
		return false;
	}

	stbi_set_flip_vertically_on_load(true);

	int width, height, nComponents;
	heightData = stbi_load("../resources/texture/terrain/heightMap.png", &width, &height, &nComponents, 0);

	if (heightData == nullptr || width == 0 || height == 0)
		return false;

	this->width  = width;
	this->height = height;

	stbi_set_flip_vertically_on_load(false);

	return true;
}

/**
* @ brief		bake terrain map 
* @ details		bake terrain map with 4 components( xyz : normal vector, w : height ). 
* @ param		resolutionX means width of baked map.
* @ param		resolutionY means height of baked map.
* @ param		aspectRatio is needed for getting perspective matrix.
* @ todo		save baked texture to file which can have float value.
*/
void EngineTerrain::bakeTerrainMap(int resolutionX, int resolutionY, float aspectRatio)
{
	GLShader bakeNormalMap, bakeTerrainMap;

	try
	{
		bakeNormalMap.loadAsset({ "../resources/shader/bakeNormalMap_vs.glsl", "../resource/shader/bakeNormalMap_fs.glsl" });
		bakeTerrainMap.loadAsset({ "../resources/shader/bakeTerrainMap_vs.glsl", "../resource/shader/bakeTerrainMap_fs.glsl" });
	}
	catch (std::exception e)
	{
		EngineLogger::getConsole()->error("Failed to bake terrain map (open shader fail)");
		return;
	}

	bakeNormalMap.sendUniform("heightMap", 0);
	bakeTerrainMap.sendUniform("normalMap", 0);
	bakeTerrainMap.sendUniform("heightMap", 1);

	//simple quad for baking
	GLfloat vertices[] = 
	{
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f
	};

	GLuint quadVAO, quadVBO;
	glGenVertexArrays(1u, &quadVAO);
	glGenBuffers(1u, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0u);
	glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)0);
	glEnableVertexAttribArray(1u);
	glVertexAttribPointer(1u, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)(sizeof(GLfloat) * 3));
	glBindVertexArray(0u);
	
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.f), aspectRatio, 0.1f, 10.0f);
	glm::mat4 captureView = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	unsigned int captureFBO, captureRBO;

	glGenFramebuffers(1u, &captureFBO);
	glGenRenderbuffers(1u, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolutionX, resolutionY);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	
	GLuint heightMap;
	if ((heightMap = GLResources::CreateTexture2D("../resources/texture/terrain/heightMap.png", false)) == 0)
	{
		EngineLogger::getConsole()->error("Failed to Bake terrain map (cannot open heightMap)");
		return;
	}

	GLuint normalMap;
	glGenTextures(1u, &normalMap);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, resolutionX, resolutionY, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalMap, 0);

	glViewport(0, 0, resolutionX, resolutionY);
	bakeNormalMap.useProgram();
	bakeNormalMap.sendUniform("view", captureView);
	bakeNormalMap.sendUniform("projection", captureProjection);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMap);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4u);
	glBindVertexArray(0u);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1u, &terrainMap);
	glBindTexture(GL_TEXTURE_2D, terrainMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, resolutionX, resolutionY, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolutionX, resolutionY);
	glFramebufferTexture2D(GL_TEXTURE_2D, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, terrainMap, 0);
	
	glViewport(0, 0, resolutionX, resolutionY);
	bakeTerrainMap.useProgram();
	bakeTerrainMap.sendUniform("view", captureView);
	bakeTerrainMap.sendUniform("projection", captureProjection);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, heightMap);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4u);
	glBindVertexArray(0u);

	glBindFramebuffer(GL_FRAMEBUFFER, 0u);

	//bake texture and save it to file
	//glBindTexture(GL_TEXTURE_2D, terrainMap);
	//glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, ) 

	glDeleteTextures(1u, &heightMap);
	glDeleteTextures(1u, &normalMap);
	glDeleteRenderbuffers(1u, &captureRBO);
	glDeleteFramebuffers(1u, &captureFBO);
	glDeleteBuffers(1u, &quadVBO);
	glDeleteVertexArrays(1u, &quadVAO);
}