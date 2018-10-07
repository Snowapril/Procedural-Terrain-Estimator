#include "LightSourceWrapper.hpp"
#include <glad/glad.h>
#include "GLShader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "GLResources.hpp"
#include "EngineCamera.hpp"
#include <imgui/imgui.h>

#ifndef _DEBUG
#include "PostprocessShaderCode.hpp"
#endif

constexpr uint32_t MAX_DIR_LIGHT = 5;

LightSourceWrapper::LightSourceWrapper()
{
}

LightSourceWrapper::~LightSourceWrapper()
{
}

bool LightSourceWrapper::initDepthPassBuffer(int width, int height)
{
	depthPassBuffer = std::make_unique<GLFramebuffer>();

	depthPassBuffer->initFramebuffer();
	depthPassBuffer->attachDepthbuffer(width, height);

	if (!depthPassBuffer->configureFramebuffer())
		return false;

#ifdef _DEBUG
	sunShader = make_unique_from_list<GLShader, std::string>({
		"../resources/shader/sun_vs.glsl",
		"../resources/shader/sun_fs.glsl",
	});
#else
	sunShader = std::make_unique<GLShader>();
	sunShader->loadAssetRaw(SUN_VS, SUN_FS);
#endif

	sunShader->useProgram();
	sunShader->sendUniform("effectTexture", 0);

	sunMesh.initWithFixedShape(MeshShape::QUAD_TRIANGLE_STRIP, 300.0f);

	if ((sunTexture = GLResources::CreateTexture2D("../resources/texture/lensFlare/sun.png", true)) == 0)
		return false;

	return true;
}

bool LightSourceWrapper::addDirLight(const glm::vec3 & dir, const glm::vec3 & color)
{
	if (dirLights.size() >= MAX_DIR_LIGHT)
		return false;

	dirLights.push_back({ dir, color });

	return true;
}

void LightSourceWrapper::renderSun(const EngineCamera& camera) const
{
	sunShader->useProgram();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sunTexture);

	camera.sendVP(*sunShader);

	const DirectionLight& dirLight = dirLights[0];

	glm::mat4 model(1.0f);
	model = glm::translate(model, -dirLight.direction);

	sunShader->sendUniform("model", model);
	sunShader->sendUniform("sunColor", dirLight.color);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);

	sunMesh.drawMesh(GL_TRIANGLE_STRIP);

	glDepthFunc(GL_LESS);
	glDisable(GL_BLEND);
}

void LightSourceWrapper::sendLightSources(const GLShader & shader) const
{
	shader.useProgram();
	
	if (dirLights.size() == 1)
	{
		shader.sendUniform("dirLight.direction", dirLights[0].direction);
		shader.sendUniform("dirLight.color", dirLights[0].color);
	}
	else
	{
		for (uint32_t i = 0; i < dirLights.size(); ++i)
		{
			shader.sendUniform("dirLight[" + std::to_string(i) + "].direction", dirLights[i].direction);
			shader.sendUniform("dirLight[" + std::to_string(i) + "].color", dirLights[i].color);
		}
	}
}

void LightSourceWrapper::updateGUI(void)
{
	if (ImGui::TreeNode("Light Property"))
	{
		ImGui::DragFloat3("Direction", &dirLights[0].direction[0], 10.0f, -5000.0f, 5000.0f);
		ImGui::ColorEdit3("Light Color", &dirLights[0].color[0]);

		ImGui::TreePop();
	}
}