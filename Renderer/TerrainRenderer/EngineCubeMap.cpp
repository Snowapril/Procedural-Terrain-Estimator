#include "EngineCubeMap.hpp"
#include "GLShader.hpp"
#include "AssetManager.hpp"
#include "EngineLogger.hpp"
#include <glad/glad.h>
#include "GLResources.hpp"

#include "GLDebugger.hpp"

EngineCubeMap::EngineCubeMap()
	: cubeMap(0)
{
}

EngineCubeMap::EngineCubeMap(const std::string& cubeMapDir, const std::string& extension)
{
	initCubeMap(cubeMapDir, extension);
}

EngineCubeMap::~EngineCubeMap()
{
	if (cubeMap)
		glDeleteTextures(1, &cubeMap);
}

bool EngineCubeMap::initCubeMap(const std::string& cubeMapDir, const std::string& extension)
{
	assetManager = std::make_unique<AssetManager>();

	try
	{
		skyboxShader = assetManager->addAsset<GLShader, std::string>({
			"../resources/shader/skybox_vs.glsl",
			"../resources/shader/skybox_fs.glsl"
			});
	}
	catch (std::exception e)
	{
		EngineLogger::getConsole()->error("Intializing Skybox failed. ({})", e.what());
		return false;
	}

	skyboxShader->sendUniform("cubeMap", 0);

	if (!skyboxMesh.initWithFixedShape(MeshShape::CUBE_TRIANGLES))
		return false;

	return true;
}