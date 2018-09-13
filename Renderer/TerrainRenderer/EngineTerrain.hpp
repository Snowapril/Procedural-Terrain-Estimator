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
#include "GLTexture.hpp"
#include "DynamicTerrain.hpp"

class AssetManager;
class GLShader;

class EngineTerrain : public DynamicTerrain
{
private:
	unsigned int terrainMap;
	unsigned int splatMap;

	GLShader* terrainShader;
	std::unique_ptr<AssetManager> assetManager;
	glm::vec3 prevCameraPos;

	GLTexture tileTextures;
	static bool isInstanciated;
public:
	EngineTerrain() = default;
	EngineTerrain(std::initializer_list<std::string>&& paths);
	virtual ~EngineTerrain();
	EngineTerrain(const EngineTerrain& other) = delete;
	EngineTerrain& operator=(const EngineTerrain& other) = delete;
public:
	bool initTerrain(std::initializer_list<std::string>&& paths);

	void buildNonUniformPatch(const glm::vec3& cameraPos, const glm::vec3& originPos) noexcept;
	void updateScene(float dt);

	void drawScene(unsigned int drawMode) const;
	void bakeTerrainMap(void);
};

#endif