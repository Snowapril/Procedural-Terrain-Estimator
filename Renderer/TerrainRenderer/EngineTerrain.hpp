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
#include <glm/mat4x4.hpp>
#include "GLTexture.hpp"

template <typename T>
using uPtr = std::unique_ptr<T>;

template <typename T>
using iList = std::initializer_list<T>;

class EngineCamera;
class AssetManager;
class GLShader;
class LightSourceWrapper;
class EngineWater;

class EngineTerrain
{
private:
	uPtr<DynamicTerrain> dynamicPatch;
	glm::mat4 biasMatrix;
	glm::vec3 terrainOriginPos;
	glm::vec3 prevCameraPos;
	glm::vec3 skycolor;
	GLTexture* terrainTexture;
	uPtr<AssetManager> terrainDataManager;
#ifdef _DEBUG
	uPtr<AssetManager> assetManager;
	GLShader* terrainShader;
	GLShader* depthPassShader;
	GLTexture* tileTextures;
	uint32_t heightmap;
#else
	uPtr<GLShader> terrainShader;
	uPtr<GLShader> depthPassShader;
	uPtr<GLTexture> tileTextures;
#endif
	float fogGradient;
	float tileSize;
	float maxHeight;

	bool enableWireFrame;
	bool enableTriangleNormal;
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

	void updateScene(float dt, EngineWater& water, const glm::vec3& cameraPos);
	void updateGUI(void);

	void drawScene_DepthPass(const EngineCamera& camera, const LightSourceWrapper& lightWrapper, bool occludePass, const glm::vec4& clipPlane) const;
	void drawScene(const EngineCamera& camera, const LightSourceWrapper& lightWrapper, const glm::vec4& clipPlane) const;

	glm::vec3 getTerrainScale(void) const;
};

#endif