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
#include "DynamicTerrain.hpp"

template <typename T>
using iList = std::initializer_list<T>;

class AssetManager;
class GLShader;
class GLTexture;

class EngineTerrain : public DynamicTerrain
{
private:
	uint32_t terrainMap;
	uint32_t splatMap;

	float maxHeight;

	GLShader* terrainShader;
	std::unique_ptr<AssetManager> assetManager;
	glm::vec3 prevCameraPos;
	glm::vec3 terrainOriginPos;

	GLTexture* tileTextures;
	static bool isInstanciated;
protected:
	bool bakeTerrainMap(void);
public:
	EngineTerrain() = default;
	EngineTerrain(const glm::vec3& position, iList<std::string>&& paths);
	virtual ~EngineTerrain();
	EngineTerrain(const EngineTerrain& other) = delete;
	EngineTerrain& operator=(const EngineTerrain& other) = delete;
public:
	static float getProperMaxHeight(std::size_t width, std::size_t height);

	bool initTerrain(const glm::vec3& position, iList<std::string>&& paths);

	void updateScene(float dt, const glm::vec3& cameraPos);
	void drawScene(uint32_t drawMode, const glm::vec4& clipPlane) const;

	glm::vec3 getTerrainScale(void) const;
};

#endif