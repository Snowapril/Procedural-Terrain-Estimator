#include "EngineWater.hpp"
#include <cassert>
#include <glad/glad.h>
#include "EngineLogger.hpp"
#include "GLShader.hpp"
#include "AssetManager.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

bool EngineWater::isInstanciated = false;

EngineWater::EngineWater()
	: position(0.0f), scale(100.0f, 1.0f, 100.0f)
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

void EngineWater::updateWater(float dt)
{

}

void EngineWater::drawWater(unsigned int drawMode) const
{
	glm::mat4 model(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, scale);

	waterShader->useProgram();
	waterShader->sendUniform("model", model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, reflectionFBO.getColorTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refractionFBO.getColorTexture());

	waterMesh.drawMesh(GL_TRIANGLE_STRIP);
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
		waterShader = assetManager->addAsset<GLShader>({
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