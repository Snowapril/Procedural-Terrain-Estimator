#include "EngineWater.hpp"
#include <cassert>
#include <glad/glad.h>
#include "EngineLogger.hpp"
#include "GLShader.hpp"
#include "AssetManager.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include "GLResources.hpp"
#include "LightSourceWrapper.hpp"
#include "EngineCamera.hpp"

bool EngineWater::isInstanciated = false;

constexpr float		WAVE_SPEED		= 0.04f;
constexpr glm::vec3 LIGHT_POSITION	= glm::vec3(500.0f, 100.0f, 500.0f);
constexpr glm::vec3 LIGHT_COLOR		= glm::vec3(0.8f, 0.8f, 0.8f);

EngineWater::EngineWater()
	: moveFactor(0.0f), tiling(6.0f), position(0.0f), scale(100.0f, 1.0f, 100.0f)
{
	assert(!isInstanciated);
	isInstanciated = true;
}

EngineWater::EngineWater(const glm::vec3& position, const glm::vec3& scale)
	: position(position), scale(scale)
{
	assert(!isInstanciated);
	isInstanciated = true;
}

EngineWater::~EngineWater()
{
	isInstanciated = false;
}

bool EngineWater::initWater(int reflectionWidth, int reflectionHeight, int refractionWidth, int refractionHeight)
{
	if (!initFramebuffers(reflectionWidth, reflectionHeight, refractionWidth, refractionHeight))
		return false;

	if (!initShaders())
		return false;

	if ((dudvMap = GLResources::CreateTexture2D("../resources/texture/water/dudvMap.png", false)) == 0)
		return false;

	if ((normalMap = GLResources::CreateTexture2D("../resources/texture/water/normalMap.png", false)) == 0)
		return false;

	EngineLogger::getConsole()->info("Initializing Water finished");
	return true;
}

void EngineWater::updateWater(float dt)
{
	if (assetManager->refreshDirtyAssets())
	{
		waterShader->useProgram();
		waterShader->sendUniform("reflectionTexture", 0);
		waterShader->sendUniform("refractionTexture", 1);
		waterShader->sendUniform("dudvMap", 2);
		waterShader->sendUniform("normalMap", 3);
		waterShader->sendUniform("depthMap", 4);
	}
	
	moveFactor += WAVE_SPEED * dt;
	if (moveFactor > 1.0f)
		moveFactor -= 1.0f;

	waterShader->useProgram();
	waterShader->sendUniform("moveFactor", moveFactor);
	waterShader->sendUniform("tiling", tiling);
}

void EngineWater::drawWater(const EngineCamera& camera, const LightSourceWrapper& lightWrapper) const
{
	glm::mat4 model(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, scale);

	waterShader->useProgram();
	waterShader->sendUniform("model", model);
	waterShader->sendUniform("viewPos", camera.getViewPos());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, reflectionFBO.getColorTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refractionFBO.getColorTexture());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, dudvMap);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, refractionFBO.getDepthTexture());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	lightWrapper.sendLightSources(*waterShader);

	camera.sendVP(*waterShader, false);
	waterMesh.drawMesh(GL_TRIANGLE_STRIP);

	glDisable(GL_BLEND);
}

bool EngineWater::initFramebuffers(int reflectionWidth, int reflectionHeight, int refractionWidth, int refractionHeight)
{
	reflectionFBO.initFramebuffer();
	reflectionFBO.attachColorTexture(reflectionWidth, reflectionHeight, GL_REPEAT, false);
	reflectionFBO.attachDepthbuffer(reflectionWidth, reflectionHeight);
	reflectionFBO.attachDepthTexture(reflectionWidth, reflectionHeight, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT);

	if (!reflectionFBO.configureFramebuffer())
	{
		EngineLogger::getConsole()->error("Reflection Framebuffer is not completed");
		return false;
	}

	refractionFBO.initFramebuffer();
	refractionFBO.attachColorTexture(refractionWidth, refractionHeight, GL_REPEAT, false);
	refractionFBO.attachDepthbuffer(refractionWidth, refractionHeight);
	refractionFBO.attachDepthTexture(refractionWidth, refractionHeight, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT);

	if (!refractionFBO.configureFramebuffer())
	{
		EngineLogger::getConsole()->error("Refraction Framebuffer is not completed");
		return false;
	}

	EngineLogger::getConsole()->info("initializing Water framebuffers finished");
	return true;
}

bool EngineWater::initShaders()
{
	assetManager = std::make_unique<AssetManager>();
	
	try
	{
		waterShader = assetManager->addAsset<GLShader, std::string>({
			"../resources/shader/water_vs.glsl",
			"../resources/shader/water_fs.glsl",
		});
	}
	catch (std::exception e)
	{
		EngineLogger::getConsole()->error("Initializing Shaders at EngineWater Failed.");
		return false;
	}

	waterShader->useProgram();
	waterShader->sendUniform("reflectionTexture", 0);
	waterShader->sendUniform("refractionTexture", 1);
	waterShader->sendUniform("dudvMap", 2);
	waterShader->sendUniform("normalMap", 3);
	waterShader->sendUniform("depthMap", 4);

	if (!waterMesh.initWithFixedShape(MeshShape::QUAD_TRIANGLE_STRIP))
		return false;

	return true;
}

void EngineWater::unbindCurrentFramebuffer(int width, int height) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void EngineWater::setTransform(glm::vec3 pos, glm::vec3 scale)
{
	this->position	= std::move(pos);
	this->scale		= std::move(scale);
}