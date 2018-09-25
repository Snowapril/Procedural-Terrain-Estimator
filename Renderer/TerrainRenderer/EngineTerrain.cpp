#include "EngineTerrain.hpp"
#include "GLShader.hpp"
#include "GLResources.hpp"
#include "AssetManager.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "EngineLogger.hpp"
#include "GLMesh.hpp"
#include "TerrainPatch.hpp"

#include <stb/stb_image.h>

bool EngineTerrain::isInstanciated = false;

EngineTerrain::EngineTerrain(const glm::vec3& position, std::initializer_list<std::string>&& paths)
	: DynamicTerrain(), terrainMap(0), terrainShader(nullptr), prevCameraPos(-1.f)
{
	assert(!isInstanciated);
	isInstanciated = true;

	initTerrain(position, std::move(paths));
}

EngineTerrain::~EngineTerrain()
{
	glDeleteTextures(1, &terrainMap);
	glDeleteTextures(1, &splatMap);

	isInstanciated = false;
}

void EngineTerrain::updateScene(float dt, const glm::vec3& cameraPos)
{
	if (assetManager->refreshDirtyAssets())
	{
		terrainShader->useProgram();
		terrainShader->sendUniform("originPos", this->terrainOriginPos);
		terrainShader->sendUniform("terrainMap", 0);
		terrainShader->sendUniform("splatMap", 1);
		terrainShader->sendUniform("dirtTexture", 2);
		terrainShader->sendUniform("rockTexture", 3);
		terrainShader->sendUniform("grassTexture", 4);
		terrainShader->sendUniform("terrainScale", glm::vec2(width, height));
		terrainShader->sendUniform("terrainMaxHeight", maxHeight);
	}

	if (cameraPos == prevCameraPos)
		return;

	DynamicTerrain::updateTerrain(cameraPos);

	prevCameraPos = cameraPos;
}

/**
* @ brief		draw Terrain with tessellation (LODing technique)
*/
void EngineTerrain::drawScene(unsigned int drawMode, const glm::vec4& clipPlane) const
{
	terrainShader->useProgram();
	terrainShader->sendUniform("clipPlane", clipPlane);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, splatMap);
	tileTextures.applyTexture();

	terrainShader->sendUniform("wireColor", glm::vec3(1.0f, 1.0f, 1.0f));
	DynamicTerrain::drawTerrain(drawMode);
}


/**
* @ brief		init context for rendering terrain with local files(height map, normal map, etc ..)
* @ details		initialize resources which is needed for rendering terrain with local files pre-compared.
advantage of this approach is 'speed', bacause height map and normal map, etc .. are precompared, thus
no need to generate in run-time.
disadvantage of this approach is "will generate fixed result".
* @ return		return boolean whether if initialization is successful or not.
*/
bool EngineTerrain::initTerrain(const glm::vec3& position, std::initializer_list<std::string>&& paths)
{
	assetManager = std::make_unique<AssetManager>();
	try
	{
		terrainShader = assetManager->addAsset<GLShader>({
			"../resources/shader/terrain_vs.glsl",
			"../resources/shader/terrain_tcs.glsl",
			"../resources/shader/terrain_tes.glsl",
			"../resources/shader/terrain_fs.glsl",
		});
	}
	catch (std::exception e)
	{
		EngineLogger::getConsole()->error("Failed to init EngineTerrain (cannot open shader or compile failed)");
		return false;
	}

	bakeTerrainMap();
	if ((splatMap = GLResources::CreateTexture2D("../resources/texture/terrain/splatMap.png", false)) == 0)
		return false;

	try
	{
		tileTextures.loadAsset({ std::make_pair<unsigned int, std::string>(2, "../resources/texture/terrain/dirt.jpg"),
								 std::make_pair<unsigned int, std::string>(3, "../resources/texture/terrain/rock.jpg"),
								 std::make_pair<unsigned int, std::string>(4, "../resources/texture/terrain/grass.png") });
	}
	catch (std::exception e)
	{
		EngineLogger::getConsole()->error("Failed to initialize EngineTerrain");
	}

	if (!initDynamicTerrain(position))
		return false;

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
void EngineTerrain::bakeTerrainMap(void)
{
	GLShader bakeTerrainMap;
	try
	{
		bakeTerrainMap.loadAsset({ "../resources/shader/bakeTerrainMap_vs.glsl", "../resources/shader/bakeTerrainMap_fs.glsl" });
	}
	catch (std::exception e)
	{
		EngineLogger::getConsole()->error("Failed to bake terrain map (open shader fail) : {}", e.what());
		return;
	}

	bakeTerrainMap.useProgram();
	bakeTerrainMap.sendUniform("heightMap", 0);

	unsigned int heightMap;
	if ((heightMap = GLResources::CreateTexture2D("../resources/texture/terrain/lakeMap.jpg", width, height, false)) == 0)
		return;
	
	maxHeight = getProperMaxHeight(width, height);
	bakeTerrainMap.sendUniform("terrainMaxHeight", maxHeight);

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

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureView = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	unsigned int captureFBO, captureRBO;

	glGenFramebuffers(1u, &captureFBO);
	glGenRenderbuffers(1u, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, terrainMap, 0);

	glGenTextures(1u, &terrainMap);
	glBindTexture(GL_TEXTURE_2D, terrainMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, terrainMap, 0);

	glViewport(0, 0, width, height);
	bakeTerrainMap.useProgram();
	bakeTerrainMap.sendUniform("view", captureView);
	bakeTerrainMap.sendUniform("projection", captureProjection);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMap);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4u);
	glBindVertexArray(0u);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDeleteRenderbuffers(1u, &captureRBO);
	glDeleteFramebuffers(1u, &captureFBO);
	glDeleteBuffers(1u, &quadVBO);
	glDeleteVertexArrays(1u, &quadVAO);
	glDeleteTextures(1u, &heightMap);
}

float EngineTerrain::getProperMaxHeight(std::size_t width, std::size_t height)
{
	float multiplier = std::log2(min(width, height));

	return static_cast<float>(multiplier * multiplier);
}

glm::vec3 EngineTerrain::getTerrainScale(void) const
{
	return glm::vec3(width, maxHeight, height);
}