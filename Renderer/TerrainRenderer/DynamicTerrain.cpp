#include "DynamicTerrain.hpp"
#include "GLShader.hpp"
#include "GLResources.hpp"
#include "AssetManager.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "EngineLogger.hpp"
#include "GLMesh.hpp"
#include "TerrainPatch.hpp"

#include "EngineProperty.hpp"
#include "obfuscator.hpp"

constexpr std::size_t	MAX_POOL_SIZE		 = 3000;
constexpr float			MIN_PATCH_LENGTH	 = 16.0f;
constexpr float			DIVIDE_HUDDLE		 = 2.8f;
DynamicTerrain::DynamicTerrain()
	: width(0), height(0), rootPatch(nullptr), tailPatch(nullptr)
{
}

DynamicTerrain::~DynamicTerrain()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);

	tailPatch = rootPatch;
	rootPatch = nullptr;

	if (tailPatch)
		delete[] tailPatch;
}

bool DynamicTerrain::initDynamicTerrain(const glm::vec3& position)
{
	try
	{
		tailPatch = new TerrainPatch[MAX_POOL_SIZE];
		rootPatch = tailPatch;
	}
	catch (std::bad_alloc e)
	{
		EngineLogger::getConsole()->error(OBFUSCATE("Bad alloc is occurred at pre-instanciate terrain patch. size : {}"), sizeof(TerrainPatch) * MAX_POOL_SIZE);
		return false;
	}

	vertices.reserve(MAX_POOL_SIZE * 16);
	terrainCenterPos = position;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.capacity() * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 3));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

void DynamicTerrain::updateTerrain(const glm::vec3& cameraPos)
{
	tailPatch = rootPatch;

	clearTree();
	createTree(cameraPos);

	traverseQuadtree(rootPatch);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertices.size(), &vertices[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void DynamicTerrain::drawTerrain(uint32_t drawMode) const
{
	glBindVertexArray(VAO);
	glDrawArrays(drawMode, 0, vertices.size() / 4);

	glBindVertexArray(0);
}

void DynamicTerrain::clearTree(void)
{
	tailPatch = rootPatch;
	numPatch = 0;

	vertices.clear();
}

void DynamicTerrain::createTree(const glm::vec3& cameraPos)
{
	rootPatch = tailPatch++;
	++numPatch;

	rootPatch->originPos = terrainCenterPos;
	rootPatch->leftTopAdj = nullptr;
	rootPatch->rightBottomAdj = nullptr;
	rootPatch->rightTopAdj = nullptr;
	rootPatch->leftBottomAdj = nullptr;
	rootPatch->width = static_cast<float>(this->width);
	rootPatch->height = static_cast<float>(this->height);
	rootPatch->scaleNegativeX = 1.0f;
	rootPatch->scaleNegativeZ = 1.0f;
	rootPatch->scalePositiveX = 1.0f;
	rootPatch->scalePositiveZ = 1.0f;

	divideNode(rootPatch, cameraPos);
}

TerrainPatch * DynamicTerrain::createNode(TerrainPatch * parent,  const glm::vec3 & originPos, float patchWidth, float patchHeight)
{
	TerrainPatch* newNode = tailPatch++;
	++numPatch;

	newNode->originPos = originPos;
	newNode->width = patchWidth;
	newNode->height = patchHeight;
	newNode->leftBottomAdj = nullptr;
	newNode->leftTopAdj = nullptr;
	newNode->rightBottomAdj = nullptr;
	newNode->rightTopAdj = nullptr;
	newNode->scaleNegativeX = 1.0f;
	newNode->scaleNegativeZ = 1.0f;
	newNode->scalePositiveX = 1.0f;
	newNode->scalePositiveZ = 1.0f;

	return newNode;
}

void DynamicTerrain::divideNode(TerrainPatch * node, const glm::vec3 & cameraPos)
{
	const float newWidth = node->width * 0.5f;
	const float newHeight = node->height * 0.5f;

	node->leftTopAdj = createNode(node, node->originPos +
		glm::vec3(-newWidth * 0.5f, 0.0f, newHeight * 0.5f), newWidth, newHeight);

	node->rightTopAdj = createNode(node, node->originPos +
		glm::vec3(newWidth * 0.5f, 0.0f, newHeight * 0.5f), newWidth, newHeight);

	node->leftBottomAdj = createNode(node, node->originPos +
		glm::vec3(-newWidth * 0.5f, 0.0f, -newHeight * 0.5f), newWidth, newHeight);

	node->rightBottomAdj = createNode(node, node->originPos +
		glm::vec3(newWidth * 0.5f, 0.0f, -newHeight * 0.5f), newWidth, newHeight);

	bool leftTopDividable(checkDivide(node->leftTopAdj, cameraPos));
	bool rightTopDividable(checkDivide(node->rightTopAdj, cameraPos));
	bool leftBottomDividable(checkDivide(node->leftBottomAdj, cameraPos));
	bool rightBottomDividable(checkDivide(node->rightBottomAdj, cameraPos));

	if (rightTopDividable)
		divideNode(node->rightTopAdj, cameraPos);
	if (leftTopDividable)
		divideNode(node->leftTopAdj, cameraPos);
	if (leftBottomDividable)
		divideNode(node->leftBottomAdj, cameraPos);
	if (rightBottomDividable)
		divideNode(node->rightBottomAdj, cameraPos);
}

TerrainPatch* DynamicTerrain::findPatch(TerrainPatch* patch, const glm::vec3& targetPos)
{
	if (patch->originPos == targetPos)
		return patch;

	if (!patch->leftBottomAdj && !patch->leftTopAdj && !patch->rightBottomAdj && !patch->rightTopAdj)
		return patch;

	if (patch->originPos.x < targetPos.x && patch->originPos.z < targetPos.z && patch->rightTopAdj)
		return findPatch(patch->rightTopAdj, targetPos);

	if (patch->originPos.x > targetPos.x && patch->originPos.z < targetPos.z && patch->leftTopAdj)
		return findPatch(patch->leftTopAdj, targetPos);

	if (patch->originPos.x > targetPos.x && patch->originPos.z > targetPos.z && patch->leftBottomAdj)
		return findPatch(patch->leftBottomAdj, targetPos);

	if (patch->originPos.x < targetPos.x && patch->originPos.z > targetPos.z && patch->rightBottomAdj)
		return findPatch(patch->rightBottomAdj, targetPos);

	return patch;
}

void DynamicTerrain::calculateTessLevel(TerrainPatch* patch)
{
	TerrainPatch* tempPatch;

	tempPatch = findPatch(rootPatch, patch->originPos + glm::vec3(-patch->width, 0.0f, 0.0f)); //left adj search
	if (tempPatch->height > patch->height)
		patch->scaleNegativeX = 2.0f;
	
	tempPatch = findPatch(rootPatch, patch->originPos + glm::vec3(patch->width, 0.0f, 0.0f)); //right adj search
	if (tempPatch->height > patch->height)
		patch->scalePositiveX = 2.0f;

	tempPatch = findPatch(rootPatch, patch->originPos + glm::vec3(0.0f, 0.0f, patch->height)); //top adj search
	if (tempPatch->width > patch->width)
		patch->scalePositiveZ = 2.0f;

	tempPatch = findPatch(rootPatch, patch->originPos + glm::vec3(0.0f, 0.0f, -patch->height)); //bottom adj search
	if (tempPatch->width > patch->width)
		patch->scaleNegativeZ = 2.0f;
}

void DynamicTerrain::traverseQuadtree(TerrainPatch* patch)
{
	if (!patch->leftBottomAdj && !patch->leftTopAdj && !patch->rightBottomAdj && !patch->rightTopAdj)
		registerToBufferObject(patch);

	if (patch->leftBottomAdj)
		traverseQuadtree(patch->leftBottomAdj);

	if (patch->rightBottomAdj)
		traverseQuadtree(patch->rightBottomAdj);

	if (patch->leftTopAdj)
		traverseQuadtree(patch->leftTopAdj);

	if (patch->rightTopAdj)
		traverseQuadtree(patch->rightTopAdj);
}

void DynamicTerrain::registerToBufferObject(TerrainPatch* patch)
{
	calculateTessLevel(patch);

	glm::vec3 patchVertices[] = {
		patch->originPos + glm::vec3(patch->width / 2.0f, 0.0f, patch->height / 2.0f),
		patch->originPos + glm::vec3(patch->width / 2.0f, 0.0f, -patch->height / 2.0f),
		patch->originPos + glm::vec3(-patch->width / 2.0f, 0.0f, patch->height / 2.0f),
		patch->originPos + glm::vec3(-patch->width / 2.0f, 0.0f, -patch->height / 2.0f),
	};
	
	float patchLevels[] = {
		patch->scalePositiveZ,
		patch->scalePositiveX,
		patch->scaleNegativeZ,
		patch->scaleNegativeX,
	};

	for (std::size_t i = 0; i < 4; ++i)
	{
		vertices.push_back(patchVertices[i].x);
		vertices.push_back(patchVertices[i].y);
		vertices.push_back(patchVertices[i].z);
		vertices.push_back(patchLevels[i]);
	}
}

bool DynamicTerrain::checkDivide(const TerrainPatch * node, glm::vec3 cameraPos)
{
	if (min(node->width, node->height) < MIN_PATCH_LENGTH || numPatch + 4 >= MAX_POOL_SIZE)
		return false;

	if (node->width / 2.f < 1.0f || node->height / 2.f < 1.0f)
		return false;

	cameraPos.y = 0.0f;

	const float huddle = DIVIDE_HUDDLE * std::sqrt(std::pow(node->width * 0.5f, 2.0f) + std::pow(node->height * 0.5f, 2.0f));
		
	if (glm::length(cameraPos - node->originPos) > huddle)
		return false;

	return true;
}

glm::vec2 DynamicTerrain::getTerrainScale(void) const
{
	return glm::uvec2(width, height);
}

void DynamicTerrain::setTerrainScale(std::size_t _width, std::size_t _height)
{
	width = _width;
	height = _height;
}