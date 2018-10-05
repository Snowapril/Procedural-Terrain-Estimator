#include "PostProcessing.hpp"
#include <glad/glad.h>
#include "AssetManager.hpp"
#include "GLShader.hpp"
#include "EngineCamera.hpp"

bool PostProcessing::initPostProcessing(int width, int height)
{
	assetManager = std::make_unique<AssetManager>();
	try
	{
		postprocessingShader = assetManager->addAsset<GLShader, std::string>({
			"../resources/shader/postprocess_vs.glsl",
			"../resources/shader/postprocess_fs.glsl",
		});
	}
	catch (std::exception e)
	{
		return false;
	}

	postprocessingShader->useProgram();
	postprocessingShader->sendUniform("hdrBuffer", 0);
	postprocessingShader->sendUniform("depthTexture", 1);

	postprocessingBuffer = std::make_unique<GLFramebuffer>();

	postprocessingBuffer->initFramebuffer();
	postprocessingBuffer->attachColorTexture(width, height, GL_CLAMP_TO_EDGE, true);
	postprocessingBuffer->attachDepthbuffer(width, height);
	postprocessingBuffer->attachDepthTexture(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);

	if (!postprocessingBuffer->configureFramebuffer())
		return false;

	if (!framebufferMesh.initWithFixedShape(MeshShape::QUAD_TRIANGLE_STRIP))
		return false;

	return true;
}

void PostProcessing::updateScene(float dt)
{
	if (assetManager->refreshDirtyAssets())
	{
		postprocessingShader->useProgram();
		postprocessingShader->sendUniform("hdrBuffer", 0);
		postprocessingShader->sendUniform("depthTexture", 1);
	}
}

void PostProcessing::drawScene(const EngineCamera & camera) const
{
	postprocessingShader->useProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postprocessingBuffer->getColorTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, postprocessingBuffer->getDepthTexture());

	camera.sendVP(*postprocessingShader, false);
	
	glm::mat4 previousVP = camera.getPreviousVP();

	postprocessingShader->sendUniform("previousVP", previousVP);

	glDisable(GL_CULL_FACE);
	framebufferMesh.drawMesh(GL_TRIANGLE_STRIP);
	glEnable(GL_CULL_FACE);
}
