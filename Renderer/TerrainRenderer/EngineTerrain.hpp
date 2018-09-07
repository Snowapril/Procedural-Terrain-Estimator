/**
* @file		EngineTerrain.hpp
* @author	Shinjihong
* @date		27 August 2018
* @version	1.0.0
* @brief	this represent terrain itself.
* @details	init with noise map and splat map which will be provided from local (map generator).
			render with tessellation features which are available after opengl version 4.0.
* @see		GLShader
*/

#ifndef ENGINE_TERRAIN_HPP
#define ENGINE_TERRAIN_HPP

#include <memory>
#include <initializer_list>
#include <string>
#include <glm/vec3.hpp>
#include "GLMesh.hpp"
#include "TerrainPatch.hpp"
#include <vector>

class AssetManager;
class GLShader;

class EngineTerrain
{
private:
	unsigned int terrainMap;
	unsigned int VBO;
	unsigned int VAO;

	std::size_t width;
	std::size_t height;
	
	std::size_t numPatch;
	TerrainPatch* rootPatch;
	TerrainPatch* tailPatch;

	GLShader* terrainShader;

	std::vector<float> vertices;

	std::unique_ptr<AssetManager> assetManager;
	glm::vec3 prevCameraPos;
	glm::vec3 terrainCenterPos;
	static bool isInstanciated;
private:
	void clearTree(void);
	void createTree(const glm::vec3& cameraPos);
	TerrainPatch* createNode(TerrainPatch* parent, TerrainPatch::PatchType type, const glm::vec3& originPos, std::size_t patchWidth, std::size_t patchHeight);
	void divideNode(TerrainPatch* node, const glm::vec3 & cameraPos);
	bool checkDivide(const TerrainPatch* node, glm::vec3 cameraPos);
	void registerToBufferObject(const TerrainPatch* patch);
public:
	EngineTerrain();
	~EngineTerrain();
	EngineTerrain(const EngineTerrain& other) = delete;
	EngineTerrain& operator=(const EngineTerrain& other) = delete;
public:
	bool initWithLocalFile(float aspectRatio, std::initializer_list<std::string>&& paths);

	void buildNonUniformPatch(const glm::vec3& cameraPos, const glm::vec3& originPos) noexcept;
	void updateScene(float dt);

	void drawTerrain(unsigned int drawMode) const;
	void bakeTerrainMap(int resolutionX, int resolutionY, float aspectRatio);
};

#endif