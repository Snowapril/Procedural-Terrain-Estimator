#include "LightSourceWrapper.hpp"
#include <glad/glad.h>
#include "GLShader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "GLResources.hpp"
#include "EngineCamera.hpp"

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
	depthPassBuffer->attachDepthTexture(width, height, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT);

	if (!depthPassBuffer->configureFramebuffer())
		return false;

	sunShader = make_unique_from_list<GLShader, std::string>({
		"../resources/shader/sun_vs.glsl",
		"../resources/shader/sun_fs.glsl",
	});

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

	glm::mat4 model(1.0f);
	model = glm::translate(model, -dirLights[0].direction);

	sunShader->sendUniform("model", model);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	sunMesh.drawMesh(GL_TRIANGLE_STRIP);

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
