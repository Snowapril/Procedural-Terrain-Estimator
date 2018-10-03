#ifndef ENGINE_CUBEMAP_HPP
#define ENGINE_CUBEMAP_HPP

#include "GLMesh.hpp"
#include <memory>
#include <string>

class GLShader;
class AssetManager;
class EngineCamera;

class EngineCubeMap
{
protected:
	uint32_t cubeMap;
	GLShader* skyboxShader;
	std::unique_ptr<AssetManager> assetManager;
	GLMesh skyboxMesh;
public:
	EngineCubeMap();
	EngineCubeMap(const std::string& cubeMapDir, const std::string& extension);
	virtual ~EngineCubeMap();
public:
	virtual bool initCubeMap(const std::string& cubeMapDir, const std::string& extension);
	virtual void drawScene(const EngineCamera& camera) const = 0;
};

#endif