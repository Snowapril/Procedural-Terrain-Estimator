/**
* @file		PostProcessing.hpp
* @author	Shinjihong
* @date		06 October 2018
* @version	1.0.0
* @brief	OpenGL Post-processing
* @details	for post-processing screen-texture. at now, provide god-ray effect, gamma correction, motion-blur effect
*/

#ifndef POST_PROCESSING_HPP
#define POST_PROCESSING_HPP

#include "GLFramebuffer.hpp"
#include <memory>
#include "GLMesh.hpp"

template <typename T>
using uPtr = std::unique_ptr<T>;

class AssetManager;
class GLShader;
class EngineCamera;
class LightSourceWrapper;

class PostProcessing
{
private:

	float blurStrength;
	int numSamples;
	float gamma;
	
	float decay;
	int godRaysSamples;
	float exposure;
	float weight;

#ifdef _DEBUG
	uPtr<AssetManager> assetManager;
	GLShader* postprocessingShader;
#else
	uPtr<GLShader> postprocessingShader;
#endif
	uPtr<GLFramebuffer> postprocessingBuffer;

	GLMesh framebufferMesh;
public:
	PostProcessing() = default;
public:
	bool initPostProcessing(int width, int height);

	void updateScene(float dt);
	void drawScene(const EngineCamera& camera, const LightSourceWrapper& lightWrapper, uint32_t occludeTexture) const;
	void updateGUI(void);
	inline void bindPostprocessing(int width, int height) const
	{
		postprocessingBuffer->bindFramebuffer(width, height);
	}
	inline void unbindPostprocessing(int width, int height) const
	{
		postprocessingBuffer->unbindFramebuffer(width, height);
	}
	inline uint32_t getOffscreenColorTexture(void) const
	{
		return postprocessingBuffer->getColorTexture();
	}
	inline uint32_t getOffscreenDepthTexture(void) const
	{
		return postprocessingBuffer->getDepthTexture();
	}
};


#endif