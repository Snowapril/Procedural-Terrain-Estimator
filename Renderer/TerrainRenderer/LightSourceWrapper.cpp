#include "LightSourceWrapper.hpp"
#include <glad/glad.h>
#include "GLShader.hpp"
#include <glm/gtc/matrix_transform.hpp>

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
	depthPassBuffer->attachDepthTexture(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);

	if (!depthPassBuffer->configureFramebuffer())
		return false;

	return true;
}

bool LightSourceWrapper::addDirLight(const glm::vec3 & dir, const glm::vec3 & color)
{
	if (dirLights.size() >= MAX_DIR_LIGHT)
		return false;

	dirLights.push_back({ glm::normalize(dir), color });

	return true;
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
