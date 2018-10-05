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

class PostProcessing
{
private:
	GLShader* postprocessingShader;

	uPtr<AssetManager> assetManager;
	uPtr<GLFramebuffer> postprocessingBuffer;

	GLMesh framebufferMesh;
public:
	PostProcessing() = default;
public:
	bool initPostProcessing(int width, int height);

	void updateScene(float dt);
	void drawScene(const EngineCamera& camera) const;

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