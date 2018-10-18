/**
* @file		EngineWater.hpp
* @author	Shinjihong
* @date		20 September 2018
* @version	1.0.0
* @brief	OpenGL Water Rendering
* @details	Provide several properties and method for rendering water. at first, we need get reflection tetxure by binding reflection FBO and second, get refraction texture
			by binding refraction FBO. finally we can render perfect water by calling drawWater method.
*/

#ifndef ENGINE_WATER_HPP
#define ENGINE_WATER_HPP

#include "GLFramebuffer.hpp"
#include <glm/vec3.hpp>
#include "GLMesh.hpp"

class GLShader;
class AssetManager;
class LightSourceWrapper;
class EngineCamera;

template <typename T>
using uPtr = std::unique_ptr<T>;

class EngineWater
{
private:
	uint32_t dudvMap;
	uint32_t normalMap;

	float tiling;
	float distortionStrength;
	float shineDamper;
	float reflectivity;
	float moveFactor;

	glm::vec3 position;
	glm::vec3 scale;
	
	GLMesh waterMesh;
	
#ifdef _DEBUG
	GLShader* waterShader;
	uPtr<AssetManager> assetManager;
#else
	uPtr<GLShader> waterShader;
#endif

	GLFramebuffer reflectionFBO;
	GLFramebuffer refractionFBO;
	
	static bool isInstanciated;
protected:
	bool initShaders(void);
	bool initFramebuffers(int reflectionWidth, int reflectionHeight, int refractionWidth, int refractionHeight);
public:
	EngineWater();
	EngineWater(const glm::vec3& position, const glm::vec3& scale);
	~EngineWater();
	EngineWater(const EngineWater& other) = delete;
	EngineWater& operator=(const EngineWater& other) = delete;
public:
	inline void bindReflectionFramebuffer(int width, int height) const
	{
		reflectionFBO.bindFramebuffer(width, height);
	}
	inline void bindRefractionFramebuffer(int width, int height) const
	{
		refractionFBO.bindFramebuffer(width, height);
	}
	inline float getWaterHeight(void) const {
		return position.y;
	}
	inline uint32_t getReflectionTexture(void) const {
		return reflectionFBO.getColorTexture();
	}
	inline uint32_t getRefractionTexture(void) const {
		return refractionFBO.getColorTexture();	
	}
	inline uint32_t getRefractionDepthTexture(void) const {
		return refractionFBO.getDepthTexture();
	}

	void unbindCurrentFramebuffer(int width, int height) const;
	
	bool initWater(int reflectionWidth, int reflectionHeight, int refractionWidth, int refractionHeight);
	void setTransform(glm::vec3 pos, glm::vec3 scale);

	void updateGUI(void);
	void updateWater(float dt);
	void drawWater(const EngineCamera& camera, const LightSourceWrapper& lightWrapper) const;
};


#endif