/**
* @file		LightSourceWrapper.hpp
* @author	Shinjihong
* @date		01 October 2018
* @version	1.0.0
* @brief	Light sources(direction light only yet)
* @details	manage light sources information like position, color, etc... also provide features for rendering light sources.
*/

#ifndef LIGHT_SOURCE_WRAPPER_HPP
#define LIGHT_SOURCE_WRAPPER_HPP

#include <vector>
#include <glm/vec3.hpp>
#include <memory>
#include <stdint.h>
#include "GLFramebuffer.hpp"
#include "GLMesh.hpp"
#include <stdint.h>

template <typename T>
using uPtr = std::unique_ptr<T>;

class GLShader;
class EngineCamera;

struct DirectionLight {
	glm::vec3 direction;
	glm::vec3 color;
};

class LightSourceWrapper
{
private:
	GLMesh sunMesh;
	uPtr<GLShader> sunShader;
	uPtr<GLFramebuffer> depthPassBuffer;
	std::vector<DirectionLight> dirLights;
	uint32_t sunTexture;

public:
	LightSourceWrapper();
	~LightSourceWrapper();
public:
	bool initDepthPassBuffer(int width, int height);

	void updateGUI(void);
	void renderSun(const EngineCamera& camera) const;
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
	inline uint32_t getColorTexture(void) const
	{
		return depthPassBuffer->getColorTexture();
	}

	inline const std::vector<DirectionLight> & getDirLights(void) const
	{
		return dirLights;
	}
};

#endif