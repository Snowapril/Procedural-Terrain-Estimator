#include "EngineSkybox.hpp"
#include "GLShader.hpp"
#include "AssetManager.hpp"
#include "EngineLogger.hpp"
#include <glad/glad.h>
#include "GLResources.hpp"
#include "EngineCamera.hpp"
#include "GLDebugger.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>

constexpr float ROTATION_SPEED = 1.3f;


EngineSkybox::EngineSkybox()
	: speed(ROTATION_SPEED)
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

	speed = ROTATION_SPEED;

	return true;
}

void EngineSkybox::updateGUI(void)
{
	EngineCubeMap::updateGUI();

	if (ImGui::TreeNode("Skybox Setting"))
	{
		ImGui::SliderFloat("Rotation Speed", &speed, 0.0f, 50.0f, "%.1f");

		ImGui::TreePop();
	}
}
void EngineSkybox::updateScene(float dt)
{
	EngineCubeMap::updateScene(dt);

	rotation += speed * dt;
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