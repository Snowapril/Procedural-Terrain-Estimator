#include "LightSourceWrapper.hpp"
#include "GLShader.hpp"
#include <stdint.h>

constexpr uint32_t MAX_DIR_LIGHT = 5;

LightSourceWrapper::LightSourceWrapper()
{
}

LightSourceWrapper::~LightSourceWrapper()
{
}

bool LightSourceWrapper::addDirLight(const glm::vec3 & pos, const glm::vec3 & color)
{
	if (dirLights.size() >= MAX_DIR_LIGHT)
		return false;

	dirLights.push_back({ pos, color });

	return true;
}

void LightSourceWrapper::sendLightSources(const GLShader & shader) const
{
	shader.useProgram();
	
	if (dirLights.size() == 1)
	{
		shader.sendUniform("dirLight.position", dirLights[0].position);
		shader.sendUniform("dirLight.color", dirLights[0].color);
	}
	else
	{
		for (uint32_t i = 0; i < dirLights.size(); ++i)
		{
			shader.sendUniform("dirLight[" + std::to_string(i) + "].position", dirLights[i].position);
			shader.sendUniform("dirLight[" + std::to_string(i) + "].color", dirLights[i].color);
		}
	}
}
