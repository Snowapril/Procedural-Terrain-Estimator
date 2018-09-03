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

class AssetManager;
class GLShader;
class TerrainPatch;

class EngineTerrain
{
private:
	enum class TerrainType : int;

	unsigned int terrainMap;
	
	std::size_t width;
	std::size_t height;

	std::shared_ptr<TerrainPatch> rootPatch;
	GLShader* terrainShader;
	std::shared_ptr<AssetManager> assetManager;
	glm::vec3 prevCameraPos;
	GLMesh quadMesh;
private:
enum class TerrainType : int;
	void clearTree(const glm::vec3& originPos) noexcept;
	void createTree(const glm::vec3& originPos) noexcept;
	TerrainPatch* createNode(const TerrainPatch* parent, TerrainType type, const glm::vec3& originPos, std::size_t patchWidth, std::size_t patchHeight) noexcept;
public:
	EngineTerrain();
	~EngineTerrain();
	EngineTerrain(const EngineTerrain& other);
	EngineTerrain& operator=(const EngineTerrain& other);

public:
	bool initWithLocalFile(float aspectRatio, std::initializer_list<std::string>&& paths);

	void buildNonUniformPatch(const glm::vec3& cameraPos) noexcept;
	void drawTerrain(unsigned int drawMode) const;
	void bakeTerrainMap(int resolutionX, int resolutionY, float aspectRatio);
};

#endif