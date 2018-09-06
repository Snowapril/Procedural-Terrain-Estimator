#include "EngineTerrain.hpp"
#include "GLShader.hpp"
#include "GLResources.hpp"
#include "AssetManager.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "EngineLogger.hpp"
#include "GLMesh.hpp"
#include "TerrainPatch.hpp"

bool EngineTerrain::isInstanciated = false;

constexpr std::size_t	MAX_POOL_SIZE		 = 5000;
constexpr std::size_t	MAX_PATCH_DEPTH		 = 6;
constexpr float			MAX_DIVIDABLE_LENGTH = 100.0f;

void EngineTerrain::clearTree(void)
{
	tailPatch = rootPatch;
	numPatch  = 0;

	vertices.clear();
	depths.clear();
}

void EngineTerrain::createTree(const glm::vec3& cameraPos)
{
	rootPatch = tailPatch++;
	++numPatch;

	rootPatch->type				= TerrainPatch::PatchType::ROOT;
	rootPatch->patchDepth		= 0;
	rootPatch->originPos		= terrainCenterPos;
	rootPatch->parent			= nullptr;
	rootPatch->leftTopAdj		= nullptr;
	rootPatch->rightBottomAdj	= nullptr;
	rootPatch->rightTopAdj		= nullptr;
	rootPatch->leftBottomAdj	= nullptr;
	rootPatch->leftScale		= 1.0f;
	rootPatch->rightScale		= 1.0f;
	rootPatch->topScale			= 1.0f;
	rootPatch->bottomScale		= 1.0f;
	rootPatch->width			= this->width;
	rootPatch->height			= this->height;
	
	divideNode(rootPatch, cameraPos);
}

TerrainPatch * EngineTerrain::createNode(TerrainPatch * parent, TerrainPatch::PatchType type, const glm::vec3 & originPos, std::size_t patchWidth, std::size_t patchHeight)
{
	TerrainPatch* newNode = tailPatch++;
	++numPatch;

	newNode->type			= type;
	newNode->patchDepth		= parent->patchDepth + 1;
	newNode->parent			= parent;
	newNode->originPos		= originPos;
	newNode->width			= patchWidth;
	newNode->height			= patchHeight;
	newNode->leftBottomAdj	= nullptr;
	newNode->leftTopAdj		= nullptr;
	newNode->rightBottomAdj = nullptr;
	newNode->rightTopAdj	= nullptr;
	newNode->leftScale		= 1.0f;
	newNode->topScale		= 1.0f;
	newNode->rightScale		= 1.0f;
	newNode->bottomScale	= 1.0f;
	
	return newNode;
}

void EngineTerrain::divideNode(TerrainPatch * node, const glm::vec3 & cameraPos)
{
	const float newWidth = node->width * 0.5f;
	const float newHeight = node->height * 0.5f;

	node->leftTopAdj	 = createNode(node, TerrainPatch::PatchType::LEFT_TOP, node->originPos + 
						 			glm::vec3(-newWidth * 0.5f, 0.0f, newHeight * 0.5f), newWidth, newHeight);
						 
	node->rightTopAdj	 = createNode(node, TerrainPatch::PatchType::RIGHT_TOP, node->originPos + 
						 			glm::vec3(newWidth * 0.5f, 0.0f, newHeight * 0.5f), newWidth, newHeight);
						 
	node->leftBottomAdj	 = createNode(node, TerrainPatch::PatchType::LEFT_BOTTOM, node->originPos + 
						 			glm::vec3(-newWidth * 0.5f, 0.0f, -newHeight * 0.5f), newWidth, newHeight);
						 
	node->rightBottomAdj = createNode(node, TerrainPatch::PatchType::RIGHT_BOTTOM, node->originPos + 
										glm::vec3(newWidth * 0.5f, 0.0f, -newHeight * 0.5f), newWidth, newHeight);


	bool leftTopDividable(checkDivide(node->leftTopAdj, cameraPos));
	bool rightTopDividable(checkDivide(node->rightTopAdj, cameraPos));
	bool leftBottomDividable(checkDivide(node->leftBottomAdj, cameraPos));
	bool rightBottomDividable(checkDivide(node->rightBottomAdj, cameraPos));

	//bool dividable = leftTopDividable || rightTopDividable || leftBottomDividable || rightBottomDividable;

	if (leftTopDividable)
		divideNode(node->leftTopAdj, cameraPos);
	else
		registerToBufferObject(node->leftTopAdj);
	if (rightTopDividable)
		divideNode(node->rightTopAdj, cameraPos);
	else
		registerToBufferObject(node->rightTopAdj);
	if (leftBottomDividable)
		divideNode(node->leftBottomAdj, cameraPos);
	else
		registerToBufferObject(node->leftBottomAdj);
	if (rightBottomDividable)
		divideNode(node->rightBottomAdj, cameraPos);
	else
		registerToBufferObject(node->rightBottomAdj);

	//if (!dividable)
	//	registerToBufferObject(node);
}

void EngineTerrain::registerToBufferObject(const TerrainPatch* patch)
{
	glm::vec3 patchVertices[] = {
		patch->originPos + glm::vec3(patch->width / 2.0f, 0.0f, patch->height / 2.0f),
		patch->originPos + glm::vec3(patch->width / 2.0f, 0.0f, -patch->height / 2.0f),
		patch->originPos + glm::vec3(-patch->width / 2.0f, 0.0f, patch->height / 2.0f),
		patch->originPos + glm::vec3(-patch->width / 2.0f, 0.0f, -patch->height / 2.0f)
	};

	for (std::size_t i = 0; i < 4; ++i)
	{
		vertices.push_back(patchVertices[i].x);
		vertices.push_back(patchVertices[i].y);
		vertices.push_back(patchVertices[i].z);
	}

	depths.push_back(patch->patchDepth);
}

bool EngineTerrain::checkDivide(const TerrainPatch * node, glm::vec3 cameraPos)
{
	if (node->patchDepth >= MAX_PATCH_DEPTH || numPatch + 4 >= MAX_POOL_SIZE)
		return false;

	if (node->width / 2.f < 1.0f || node->height / 2.f < 1.0f)
		return false;

	cameraPos.y = node->originPos.y;

	const float huddle = std::sqrt(std::pow(node->width * 1.0f, 2.0f) + std::pow(node->height * 1.0f, 2.0f));

	if (glm::length(cameraPos - node->originPos) > huddle)
		return false;

	return true;
}

EngineTerrain::EngineTerrain()
	: terrainMap(0), width(0), height(0), rootPatch(nullptr), tailPatch(nullptr), terrainShader(nullptr)
{
	assert(!isInstanciated);
	isInstanciated = true;
}

EngineTerrain::~EngineTerrain()
{
	glDeleteTextures(1, &terrainMap);
	isInstanciated = false;

	tailPatch = rootPatch;
	rootPatch = nullptr;
	
	if (tailPatch)
		delete[] tailPatch;
}

void EngineTerrain::buildNonUniformPatch(const glm::vec3 & cameraPos, const glm::vec3& originPos) noexcept
{
	if (cameraPos == prevCameraPos)
		return;

	terrainShader->useProgram();
	terrainShader->sendUniform("originPos", originPos);
	terrainShader->sendUniform("terrainScale", glm::vec2(width, height));
	terrainShader->sendUniform("terrainMaxHeight", 5.0f);
	terrainShader->sendUniform("terrainHeightOffset", 0.0f);

	tailPatch = rootPatch;
	terrainCenterPos = originPos;

	clearTree();
	createTree(cameraPos);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertices.size(), &vertices[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(unsigned int) * depths.size(), &depths[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	prevCameraPos = cameraPos;
}

void EngineTerrain::updateScene(float dt)
{
	assetManager->refreshDirtyAssets();
}

/**
* @ brief		draw Terrain with tessellation (LODing technique)
*/
void EngineTerrain::drawTerrain(unsigned int drawMode) const
{
	terrainShader->useProgram();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainMap);

	glBindVertexArray(VAO);
	glDrawArrays(GL_PATCHES, 0, depths.size());
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
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
			//"../resources/shader/terrain_gs.glsl"
		});
	}
	catch (std::exception e)
	{
		EngineLogger::getConsole()->error("Failed to init EngineTerrain (cannot open shader or compile failed)");
		return false;
	}

	if ((terrainMap = GLResources::CreateTexture2D("../resources/texture/terrain/heightMap.jpg", width, height, false)) == 0)
		return false;

	terrainShader->useProgram();
	terrainShader->sendUniform("terrainMap", 0);

	try 
	{
		tailPatch = new TerrainPatch[MAX_POOL_SIZE];
		rootPatch = tailPatch;
	}
	catch (std::bad_alloc e)
	{
		EngineLogger::getConsole()->error("Bad alloc is occurred at pre-instanciate terrain patch. size : {}", sizeof(TerrainPatch) * MAX_POOL_SIZE);
		return false;
	}

	vertices.reserve(MAX_POOL_SIZE * MAX_PATCH_DEPTH * 12);
	depths.reserve(MAX_POOL_SIZE * MAX_PATCH_DEPTH * 1);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, &VBO[0]);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.capacity() * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
		
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, depths.capacity() * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(unsigned int), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

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