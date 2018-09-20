#include "EngineSkybox.hpp"
#include "GLShader.hpp"
#include "AssetManager.hpp"
#include "EngineLogger.hpp"
#include <glad/glad.h>
#include "GLResources.hpp"

#include "GLDebugger.hpp"

EngineSkybox::EngineSkybox()
	: cubeMap(0)
{
}

EngineSkybox::~EngineSkybox()
{
	if (cubeMap)
		glDeleteTextures(1, &cubeMap);
}

bool EngineSkybox::initSkybox(const std::string& skyboxDir, const std::string& extension)
{
	assetManager = std::make_unique<AssetManager>();
	
	try
	{
		skyboxShader = assetManager->addAsset<GLShader>({
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

	if (!skyboxMesh.initWithFixedShape(MeshShape::QUAD_TRIANGLES))
		return false;

	if ((cubeMap = GLResources::CreateCubeMap(skyboxDir, extension)) == 0)
		return false;

	return true;
}

void EngineSkybox::drawScene(unsigned int drawMode) const
{
	skyboxShader->useProgram();

	glDepthFunc(GL_LEQUAL);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	skyboxMesh.drawMesh(drawMode);
	glDepthFunc(GL_LESS);
}