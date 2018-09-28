#include "EngineSkybox.hpp"
#include "GLShader.hpp"
#include "AssetManager.hpp"
#include "EngineLogger.hpp"
#include <glad/glad.h>
#include "GLResources.hpp"

#include "GLDebugger.hpp"

EngineSkybox::EngineSkybox()
{
}

EngineSkybox::EngineSkybox(const std::string& cubeMapDir, const std::string& extension)
{
	initCubeMap(cubeMapDir, extension);
}

EngineSkybox::~EngineSkybox()
{
}

bool EngineSkybox::initCubeMap(const std::string& cubeMapDir, const std::string& extension)
{
	EngineCubeMap::initCubeMap(cubeMapDir, extension);

	if ((cubeMap = GLResources::CreateSkybox(cubeMapDir, extension)) == 0)
		return false;

	return true;
}

void EngineSkybox::drawScene(uint32_t drawMode) const
{
	skyboxShader->useProgram();

	glDepthFunc(GL_LEQUAL);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	skyboxMesh.drawMesh(drawMode);
	glDepthFunc(GL_LESS);
}