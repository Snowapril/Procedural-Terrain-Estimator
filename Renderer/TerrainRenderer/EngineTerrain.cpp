#include "EngineTerrain.hpp"
#include "GLShader.hpp"
#include "GLResources.hpp"
#include "AssetManager.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "EngineLogger.hpp"
#include "GLMesh.hpp"
#include "TerrainPatch.hpp"
#include "LightSourceWrapper.hpp"
#include "EngineCamera.hpp"
#include "EngineProperty.hpp"
#include <stb/stb_image.h>

#include <imgui/imgui.h>

#ifndef _DEBUG
#include "TerrainShaderCode.hpp"
#endif
#include "obfuscator.hpp"
#include "EngineWater.hpp"

bool EngineTerrain::isInstanciated = false;

EngineTerrain::EngineTerrain(const glm::vec3& position, iList<std::string>&& paths)
	: enableWireFrame(false), enableTriangleNormal(true), enableShadowMapping(true), terrainShader(nullptr), prevCameraPos(-1.f)
{
	assert(!isInstanciated);
	isInstanciated = true;

	initTerrain(position, std::move(paths));
}

EngineTerrain::~EngineTerrain()
{
	isInstanciated = false;
}

void EngineTerrain::updateScene(float dt, EngineWater& water, const glm::vec3& cameraPos)
{
#ifdef _DEBUG
	if (assetManager->refreshDirtyAssets())
	{
		terrainShader->useProgram();
		terrainShader->sendUniform("originPos", this->terrainOriginPos);
		terrainShader->sendUniform("terrainMap", 0);
		terrainShader->sendUniform("splatMap", 1);
		terrainShader->sendUniform("dirtTexture", 2);
		terrainShader->sendUniform("rockTexture", 3);
		terrainShader->sendUniform("grassTexture", 4);
		terrainShader->sendUniform("wetDirtTexture", 5);
		terrainShader->sendUniform("shadowMap", 6);
		terrainShader->sendUniform("terrainScale", dynamicPatch->getTerrainScale());

		depthPassShader->useProgram();
		depthPassShader->sendUniform("originPos", this->terrainOriginPos);
		depthPassShader->sendUniform("terrainMap", 0);
		depthPassShader->sendUniform("terrainScale", dynamicPatch->getTerrainScale());
		depthPassShader->sendUniform("terrainMaxHeight", maxHeight);
	}
#endif

	if (!terrainDataManager->refreshDirtyAssets())
		bakeTerrainMap();

	terrainShader->useProgram();
	terrainShader->sendUniform("terrainMaxHeight", maxHeight);
	terrainShader->sendUniform("tileSize", tileSize);
	terrainShader->sendUniform("gradient", fogGradient);
	terrainShader->sendUniform("skycolor", skycolor);
	terrainShader->sendUniform("enableTriangleNormal", enableTriangleNormal);
	terrainShader->sendUniform("enableShadowMapping", enableShadowMapping);
	terrainShader->sendUniform("terrainScale", dynamicPatch->getTerrainScale());

	depthPassShader->useProgram();
	depthPassShader->sendUniform("terrainMaxHeight", maxHeight);
	depthPassShader->sendUniform("terrainScale", dynamicPatch->getTerrainScale());
	
	glm::vec3 terrainScale = getTerrainScale() * 0.5f;
	water.setTransform(glm::vec3(0.0f, terrainScale.y * 0.5f, 0.0f), glm::vec3(terrainScale.x, 1.0f, terrainScale.z));
	dynamicPatch->updateTerrain(cameraPos);

	if (cameraPos != prevCameraPos)
	{
		prevCameraPos = cameraPos;
	}
}


void EngineTerrain::drawScene_DepthPass(const EngineCamera& camera, const LightSourceWrapper& lightWrapper, bool occludePass, const glm::vec4& clipPlane) const
{
	depthPassShader->useProgram();
	depthPassShader->sendUniform("clipPlane", clipPlane);
	depthPassShader->sendUniform("minDepth", camera.getMinDepth());
	depthPassShader->sendUniform("maxDepth", camera.getMaxDepth());
	depthPassShader->sendUniform("enableVP", occludePass);
	terrainTexture->applyTexture();

	if (occludePass)
		camera.sendVP(*depthPassShader, false);

	glm::vec2 scale = dynamicPatch->getTerrainScale();
	glm::mat4 project = glm::ortho(-scale.x * 0.5f, scale.x * 0.5f, -scale.y * 0.5f, scale.y * 0.5f, camera.getMinDepth(), camera.getMaxDepth());
	glm::mat4 view;

	const auto& dirLights = lightWrapper.getDirLights();
	
	for (const auto& dirLight : dirLights)
	{
		view = glm::lookAt(dirLight.direction, terrainOriginPos, glm::vec3(0.0f, 1.0f, 0.0f));
		
		depthPassShader->sendUniform("sunMVPMatrix", project * view);
		dynamicPatch->drawTerrain(GL_PATCHES);
	}
}

/**
* @ brief		draw Terrain with tessellation (LODing technique)
*/
void EngineTerrain::drawScene(const EngineCamera& camera, const LightSourceWrapper& lightWrapper, const glm::vec4& clipPlane) const
{
	terrainShader->useProgram();
	terrainShader->sendUniform("clipPlane", clipPlane);

	terrainTexture->applyTexture();
	tileTextures->applyTexture();
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, lightWrapper.getDepthTexture());

	terrainShader->sendUniform("minDepth", camera.getMinDepth());
	terrainShader->sendUniform("maxDepth", camera.getMaxDepth());
	lightWrapper.sendLightSources(*terrainShader);
	camera.sendVP(*terrainShader, false);

	glm::vec2 viewportScale = camera.getViewportSize();
	terrainShader->sendUniform("viewportSize", viewportScale);
	terrainShader->sendUniform("enableWireframe", enableWireFrame);
	terrainShader->sendUniform("enableTriangleNormal", enableTriangleNormal);

	glm::vec2 scale = dynamicPatch->getTerrainScale();
	glm::mat4 project = glm::ortho(-scale.x * 0.5f, scale.x * 0.5f, -scale.y * 0.5f, scale.y * 0.5f, camera.getMinDepth(), camera.getMaxDepth());
	glm::mat4 depthBiasMatrix = biasMatrix * project;
	glm::mat4 view;

	const auto& dirLights = lightWrapper.getDirLights();

	for (const auto& dirLight : dirLights)
	{
		view = glm::lookAt(dirLight.direction, terrainOriginPos, glm::vec3(0.0f, 1.0f, 0.0f));

		terrainShader->sendUniform("sunMVPMatrix", depthBiasMatrix * view);
		dynamicPatch->drawTerrain(GL_PATCHES);
	}
}

void EngineTerrain::updateGUI(void)
{
	if (ImGui::TreeNode("Terrain Setting"))
	{
		static int terrainScaleFactor = 1;
		static int prevTerrainScaleFactor = terrainScaleFactor;
		auto textureSize = terrainScaleFactor * terrainTexture->getTextureSize(0);
		float newHeight = getProperMaxHeight(textureSize.x, textureSize.y);;

		ImGui::SliderFloat("Max Height", &maxHeight, 1.0f, newHeight, "Height = %.1f");
		ImGui::SliderFloat("Tile Size", &tileSize, 1.0f, 64.f, "Size = %.1f");
		ImGui::SliderFloat("Fog Gradient", &fogGradient, 0.0f, 5.0f, "ratio = %.3f");
		ImGui::SliderInt("Terrain Scale x", &terrainScaleFactor, 1, 8, "scale x %d");
		ImGui::ColorEdit3("Fog Color", &skycolor[0]);
		ImGui::Checkbox("Wireframe", &enableWireFrame);
		ImGui::Checkbox("Triangle Normal", &enableTriangleNormal);
		ImGui::Checkbox("Shadow Mapping", &enableShadowMapping);
		
		if (terrainScaleFactor != prevTerrainScaleFactor)
		{
			dynamicPatch->setTerrainScale(textureSize.x, textureSize.y);
			maxHeight = newHeight;

			prevTerrainScaleFactor = terrainScaleFactor;
		}
		

		ImGui::TreePop();
	}
}

/**
* @ brief		init context for rendering terrain with local files(height map, normal map, etc ..)
* @ details		initialize resources which is needed for rendering terrain with local files pre-compared.
advantage of this approach is 'speed', bacause height map and normal map, etc .. are precompared, thus
no need to generate in run-time.
disadvantage of this approach is "will generate fixed result".
* @ return		return boolean whether if initialization is successful or not.
*/
bool EngineTerrain::initTerrain(const glm::vec3& position, iList<std::string>&& paths)
{
	glPatchParameteri(GL_PATCH_VERTICES, 4);

#ifdef _DEBUG
	assetManager = std::make_unique<AssetManager>();
#endif

	try
	{
#ifdef _DEBUG
		terrainShader = assetManager->addAsset<GLShader, std::string>({
			OBFUSCATE("../resources/shader/terrain_vs.glsl"),
			OBFUSCATE("../resources/shader/terrain_tcs.glsl"),
			OBFUSCATE("../resources/shader/terrain_tes.glsl"),
			OBFUSCATE("../resources/shader/terrain_gs.glsl"),
			OBFUSCATE("../resources/shader/terrain_fs.glsl"),
		});

		depthPassShader = assetManager->addAsset<GLShader, std::string>({
			OBFUSCATE("../resources/shader/terrain_vs.glsl"),
			OBFUSCATE("../resources/shader/terrain_tcs.glsl"),
			OBFUSCATE("../resources/shader/terrain_depth_pass_tes.glsl"),
			OBFUSCATE("../resources/shader/terrain_depth_pass_fs.glsl"),
		});
#else
		terrainShader = std::make_unique<GLShader>();
		terrainShader->loadAssetRaw(TERRAIN_VS, TERRAIN_FS, TERRAIN_GS, TERRAIN_TCS, TERRAIN_TES);

		depthPassShader = std::make_unique<GLShader>();
		depthPassShader->loadAssetRaw(TERRAIN_VS, TERRAIN_DEPTH_FS, nullptr, TERRAIN_TCS, TERRAIN_DEPTH_TES);
#endif
	}
	catch (std::exception e)
	{
		EngineLogger::getConsole()->error(OBFUSCATE("Failed to init EngineTerrain (cannot open shader or compile failed)"));
		return false;
	}


	terrainDataManager = std::make_unique<AssetManager>();
	try
	{
		terrainTexture = terrainDataManager->addAsset<GLTexture, std::pair<uint32_t, std::string>>({
			std::make_pair<uint32_t, std::string>(0, OBFUSCATE("../resources/texture/terrain/height16bit2.png")),
			std::make_pair<uint32_t, std::string>(1, OBFUSCATE("../resources/texture/terrain/splatMap.png")),
		});

#ifdef _DEBUG
		tileTextures = assetManager->addAsset<GLTexture, std::pair<uint32_t, std::string>>({
			std::make_pair<uint32_t, std::string>(2, OBFUSCATE("../resources/texture/terrain/dirt.jpg")),
			std::make_pair<uint32_t, std::string>(3, OBFUSCATE("../resources/texture/terrain/rock.jpg")),
			std::make_pair<uint32_t, std::string>(4, OBFUSCATE("../resources/texture/terrain/grass.png")),
			std::make_pair<uint32_t, std::string>(5, OBFUSCATE("../resources/texture/terrain/wetDirt.jpg")),
			});
#else
		tileTextures = std::make_unique<GLTexture>();
		tileTextures->loadAsset({
			std::make_pair<uint32_t, std::string>(2, OBFUSCATE("../resources/texture/terrain/dirt.jpg")),
			std::make_pair<uint32_t, std::string>(3, OBFUSCATE("../resources/texture/terrain/rock.jpg")),
			std::make_pair<uint32_t, std::string>(4, OBFUSCATE("../resources/texture/terrain/grass.png")),
			std::make_pair<uint32_t, std::string>(5, OBFUSCATE("../resources/texture/terrain/wetDirt.jpg")),
		});
#endif
	}	
	catch (std::exception e)
	{
		EngineLogger::getConsole()->error(OBFUSCATE("Failed to initialize EngineTerrain"));
		return false;
	}

	skycolor = glm::vec3(0.5f);
	tileSize = 32.0f;
	fogGradient = 2.2f;
	dynamicPatch = std::make_unique<DynamicTerrain>();

	if (!dynamicPatch->initDynamicTerrain(position))
		return false;

	if (!bakeTerrainMap())
		return false;

	biasMatrix = glm::mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	);

#ifndef _DEBUG
	terrainShader->useProgram();
	terrainShader->sendUniform(OBFUSCATE("originPos"), this->terrainOriginPos);
	terrainShader->sendUniform(OBFUSCATE("terrainMap"), 0);
	terrainShader->sendUniform(OBFUSCATE("splatMap"), 1);
	terrainShader->sendUniform(OBFUSCATE("dirtTexture"), 2);
	terrainShader->sendUniform(OBFUSCATE("rockTexture"), 3);
	terrainShader->sendUniform(OBFUSCATE("grassTexture"), 4);
	terrainShader->sendUniform(OBFUSCATE("wetDirtTexture"), 5);
	terrainShader->sendUniform(OBFUSCATE("shadowMap"), 6);
	terrainShader->sendUniform(OBFUSCATE("terrainScale"), dynamicPatch->getTerrainScale());
	terrainShader->sendUniform(OBFUSCATE("terrainMaxHeight"), maxHeight);

	depthPassShader->useProgram();
	depthPassShader->sendUniform(OBFUSCATE("originPos"), this->terrainOriginPos);
	depthPassShader->sendUniform(OBFUSCATE("terrainMap"), 0);
	depthPassShader->sendUniform(OBFUSCATE("terrainScale"), dynamicPatch->getTerrainScale());
	depthPassShader->sendUniform(OBFUSCATE("terrainMaxHeight"), maxHeight);
#endif

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
bool EngineTerrain::bakeTerrainMap(void)
{
	GLShader bakeTerrainMap, bakeShadowMap;
	try
	{
#ifdef _DEBUG
		bakeTerrainMap.loadAsset({
			OBFUSCATE("../resources/shader/bakeTerrainMap_vs.glsl"),
			OBFUSCATE("../resources/shader/bakeTerrainMap_fs.glsl")
		});
#else
		bakeTerrainMap.loadAssetRaw(BAKETERRAINMAP_VS, BAKETERRAINMAP_FS);
#endif
	}
	catch (std::exception e)
	{
		EngineLogger::getConsole()->error(OBFUSCATE("Failed to bake terrain map (open shader fail) : {}"), e.what());
		return false;
	}

	bakeTerrainMap.useProgram();
	bakeTerrainMap.sendUniform(OBFUSCATE("heightMap"), 0);

	auto textureSize = terrainTexture->getTextureSize(0);

	dynamicPatch->setTerrainScale(textureSize.x, textureSize.y);
	maxHeight = getProperMaxHeight(textureSize.x,textureSize.y);
	
	bakeTerrainMap.sendUniform(OBFUSCATE("terrainMaxHeight"), maxHeight);

	//simple quad for baking
	GLfloat vertices[] =
	{
		-1.03f,  1.03f, 0.0f, 0.0f, 1.0f,
		-1.03f, -1.03f, 0.0f, 0.0f, 0.0f,
		1.03f,  1.03f, 0.0f, 1.0f, 1.0f,
		1.03f, -1.03f, 0.0f, 1.0f, 0.0f,
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

	uint32_t captureFBO;
	uint32_t terrainMap = 0U;
	uint32_t heightMap = terrainTexture->getTextrueID(0);

	glGenFramebuffers(1u, &captureFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, terrainMap, 0);

	glGenTextures(1u, &terrainMap);
	glBindTexture(GL_TEXTURE_2D, terrainMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureSize.x, textureSize.y, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, terrainMap, 0);

	glViewport(0, 0, textureSize.x, textureSize.y);
	bakeTerrainMap.useProgram();
	bakeTerrainMap.sendUniform(OBFUSCATE("view"), captureView);
	bakeTerrainMap.sendUniform(OBFUSCATE("projection"), captureProjection);

	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMap);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4u);
	glBindVertexArray(0u);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDeleteFramebuffers(1u, &captureFBO);
	glDeleteBuffers(1u, &quadVBO);
	glDeleteVertexArrays(1u, &quadVAO);
	glDeleteTextures(1u, &heightMap);

	terrainTexture->setTextureID(0, terrainMap);

	return true;
}

float EngineTerrain::getProperMaxHeight(std::size_t width, std::size_t height)
{
	return static_cast<float>(min(width, height) * 0.25f);
}

glm::vec3 EngineTerrain::getTerrainScale(void) const
{
	glm::vec2 scale = dynamicPatch->getTerrainScale();
	return glm::vec3(scale.x, maxHeight, scale.y);
}