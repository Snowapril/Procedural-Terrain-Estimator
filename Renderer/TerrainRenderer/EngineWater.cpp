#include "EngineWater.hpp"
#include <cassert>
#include <glad/glad.h>
#include "EngineLogger.hpp"
#include "GLShader.hpp"
#include "AssetManager.hpp"

bool EngineWater::isInstanciated = false;

EngineWater::EngineWater()
	: position(0.0f), scale(100.0f, 100.0f, 1.0f)
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

	EngineLogger::getConsole()->info("Initializing Water finished");
	return true;
}

bool EngineWater::initFramebuffers(int reflectionWidth, int reflectionHeight, int refractionWidth, int refractionHeight)
{
	reflectionFBO.initFramebuffer();
	reflectionFBO.attachColorTexture(reflectionWidth, reflectionHeight);
	reflectionFBO.attachDepthTexture(reflectionWidth, reflectionHeight, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT);
	reflectionFBO.attachDepthbuffer(reflectionWidth, reflectionHeight);
	
	if (!reflectionFBO.checkFramebufferStatus())
	{
		EngineLogger::getConsole()->error("Reflection Framebuffer is not completed");
		return false;
	}

	refractionFBO.initFramebuffer();
	refractionFBO.attachColorTexture(refractionWidth, refractionHeight);
	refractionFBO.attachDepthTexture(refractionWidth, refractionHeight, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT);
	refractionFBO.attachDepthbuffer(refractionWidth, refractionHeight);

	if (!refractionFBO.checkFramebufferStatus())
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
		//waterShader = assetManager->addAsset<GLShader>({
		//	"../resources/shader/water_vs.glsl",
		//	"../resources/shader/water_fs.glsl",
		//});
	}
	catch (std::exception e)
	{
		return false;
	}

	return true;
}

void EngineWater::unbindCurrentFramebuffer(int width, int height) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
}

void EngineWater::setTransform(glm::vec3 pos, glm::vec3 scale)
{
	this->position	= std::move(pos);
	this->scale		= std::move(scale);
}