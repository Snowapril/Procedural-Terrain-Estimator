#include "PostProcessing.hpp"
#include <glad/glad.h>
#include "AssetManager.hpp"
#include "GLShader.hpp"
#include "EngineCamera.hpp"
#include "LightSourceWrapper.hpp"
#include <imgui/imgui.h>

#ifndef _DEBUG
#include "PostprocessShaderCode.hpp"
#endif

bool PostProcessing::initPostProcessing(int width, int height)
{
#ifdef _DEBUG
	assetManager = std::make_unique<AssetManager>();
#endif
	try
	{
#ifdef _DEBUG
		postprocessingShader = assetManager->addAsset<GLShader, std::string>({
			"../resources/shader/postprocess_vs.glsl",
			"../resources/shader/postprocess_fs.glsl",
		});
#else
		postprocessingShader = std::make_unique<GLShader>();
		postprocessingShader->loadAssetRaw(POSTPROCESS_VS, POSTPROCESS_FS);
#endif
	}
	catch (std::exception e)
	{
		return false;
	}

	postprocessingShader->useProgram();
	postprocessingShader->sendUniform("hdrBuffer", 0);
	postprocessingShader->sendUniform("depthTexture", 1);
	postprocessingShader->sendUniform("occludeTexture", 2);

	postprocessingBuffer = std::make_unique<GLFramebuffer>();

	postprocessingBuffer->initFramebuffer();
	postprocessingBuffer->attachColorTexture(width, height, GL_CLAMP_TO_EDGE, true);
	postprocessingBuffer->attachDepthbuffer(width, height);
	postprocessingBuffer->attachDepthTexture(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);

	blurStrength = 0.06f;
	numSamples = 7;
	gamma = 2.2;

	decay = 0.96875;
	godRaysSamples = 100;
	exposure = 0.5;
	weight = 0.5;

	if (!postprocessingBuffer->configureFramebuffer())
		return false;

	if (!framebufferMesh.initWithFixedShape(MeshShape::QUAD_TRIANGLE_STRIP))
		return false;

	return true;
}

void PostProcessing::updateGUI(void)
{
	if (ImGui::TreeNode("PostProcessing"))
	{
		if (ImGui::CollapsingHeader("Motion Blur"))
		{
			ImGui::InputFloat("Blur Strength", &blurStrength, 0.01f);
			blurStrength = Util::clamp(blurStrength, 0.0f, 1.0f);
			ImGui::SliderInt("Num Samples", &numSamples, 0, 10);
		}
		if (ImGui::CollapsingHeader("God rays"))
		{
			ImGui::InputFloat("Decay", &decay, 0.001f);
			decay = Util::clamp(decay, 0.0f, 1.0f);
			ImGui::SliderInt("Samples", &godRaysSamples, 0, 200);
			ImGui::SliderFloat("Exposure", &exposure, 0.0f, 1.0f, "%.1f");
			ImGui::SliderFloat("Weight", &weight, 0.0f, 1.0f, "%.1f");
		}
		if (ImGui::CollapsingHeader("Tone Mapping"))
		{
			ImGui::SliderFloat("GAMMA", &gamma, 0.0f, 5.0f, "%.2f");
		}
		ImGui::TreePop();
	}
}

void PostProcessing::updateScene(float dt)
{
#ifdef _DEBUG
	if (assetManager->refreshDirtyAssets())
	{
		postprocessingShader->useProgram();
		postprocessingShader->sendUniform("hdrBuffer", 0);
		postprocessingShader->sendUniform("depthTexture", 1);
		postprocessingShader->sendUniform("occludeTexture", 2);
	}
#endif

	postprocessingShader->useProgram();
	postprocessingShader->sendUniform("blurStrength", blurStrength);
	postprocessingShader->sendUniform("numSamples", numSamples);
	postprocessingShader->sendUniform("GAMMA", gamma);
	postprocessingShader->sendUniform("decay", decay);
	postprocessingShader->sendUniform("godRaysSamples", godRaysSamples);
	postprocessingShader->sendUniform("exposure", exposure);
	postprocessingShader->sendUniform("weight", weight);
}

void PostProcessing::drawScene(const EngineCamera & camera, const LightSourceWrapper& lightWrapper, uint32_t occludeTexture) const
{
	postprocessingShader->useProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postprocessingBuffer->getColorTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, postprocessingBuffer->getDepthTexture());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, occludeTexture);

	camera.sendVP(*postprocessingShader, false);
	
	glm::mat4 previousVP = camera.getPreviousVP();

	postprocessingShader->sendUniform("previousVP", previousVP);
	postprocessingShader->sendUniform("lightPosition", -lightWrapper.getDirLights()[0].direction);
	//postprocessingShader->sendUniform("viewportSize", camera.getViewportSize());

	glDisable(GL_CULL_FACE);
	framebufferMesh.drawMesh(GL_TRIANGLE_STRIP);
	glEnable(GL_CULL_FACE);
}
