#ifndef LIGHT_SOURCE_WRAPPER_HPP
#define LIGHT_SOURCE_WRAPPER_HPP

#include <vector>
#include <glm/vec3.hpp>
#include <memory>
#include <stdint.h>
#include "GLFramebuffer.hpp"

class GLShader;

struct DirectionLight {
	glm::vec3 direction;
	glm::vec3 color;
};

class LightSourceWrapper
{
private:
	std::vector<DirectionLight> dirLights;

	std::unique_ptr<GLFramebuffer> depthPassBuffer;
public:
	LightSourceWrapper();
	~LightSourceWrapper();
public:
	bool initDepthPassBuffer(int width, int height);

	bool addDirLight(const glm::vec3& dir, const glm::vec3& color);
	void sendLightSources(const GLShader& shader) const;

	inline void bindDepthPassBuffer(int width, int height) const
	{
		depthPassBuffer->bindFramebuffer(width, height);
	}

	inline void unbindDepthPassBuffer(int width, int height) const
	{
		depthPassBuffer->unbindFramebuffer(width, height);
	}

	inline uint32_t getDepthTexture(void) const
	{
		return depthPassBuffer->getDepthTexture();
	}

	inline const std::vector<DirectionLight> & getDirLights(void) const
	{
		return dirLights;
	}
};

#endif