#include "EngineSkybox.hpp"
#include "GLShader.hpp"
#include "AssetManager.hpp"
#include "EngineLogger.hpp"
#include <glad/glad.h>
#include "GLResources.hpp"
#include "EngineCamera.hpp"
#include "GLDebugger.hpp"
#include <glm/gtc/matrix_transform.hpp>

constexpr float ROTATION_SPEED = 1.3f;


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

void EngineSkybox::updateScene(float dt)
{
	rotation += ROTATION_SPEED * dt;
}

void EngineSkybox::drawScene(const EngineCamera& camera) const
{
	skyboxShader->useProgram();

	glDepthFunc(GL_LEQUAL);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	glm::mat4 project = camera.getProjectMatrix();
	glm::mat4 view = camera.getViewMatrix(true);
	
	view = glm::rotate(view, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
	
	skyboxShader->sendUniform("project", project);
	skyboxShader->sendUniform("view", view);

	skyboxMesh.drawMesh(GL_TRIANGLES);
	glDepthFunc(GL_LESS);
}