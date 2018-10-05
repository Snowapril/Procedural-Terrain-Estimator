#ifndef ENGINE_CUBEMAP_HPP
#define ENGINE_CUBEMAP_HPP

#include "GLMesh.hpp"
#include <memory>
#include <string>

template <typename T>
using uPtr = std::unique_ptr<T>;

class GLShader;
class AssetManager;
class EngineCamera;

class EngineCubeMap
{
protected:
	uint32_t cubeMap;
	GLShader* skyboxShader;
	uPtr<AssetManager> assetManager;
	GLMesh skyboxMesh;
public:
	EngineCubeMap();
	EngineCubeMap(const std::string& cubeMapDir, const std::string& extension);
	virtual ~EngineCubeMap();
public:
	virtual void updateScene(float dt) = 0;
	virtual bool initCubeMap(const std::string& cubeMapDir, const std::string& extension);
	virtual void drawScene(const EngineCamera& camera) const = 0;
};

#endif