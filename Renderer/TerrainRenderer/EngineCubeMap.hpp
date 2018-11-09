/**
* @file		EngineCubeMap.hpp
* @author	Shinjihong
* @date		28 September 2018
* @version	1.0.0
* @brief	OpenGL Cube map parent class.
* @details	this class is for rendering Cube map (skybox or HDR environment map(is also skybox)). provide method for initialize skybox with directory and extension parameter.
			provide update and render method like other SceneObject classes.
* @see
*/

#ifndef ENGINE_CUBEMAP_HPP
#define ENGINE_CUBEMAP_HPP

#include "GLMesh.hpp"
#include <memory>
#include <string>
#include <glm/vec3.hpp>

template <typename T>
using uPtr = std::unique_ptr<T>;

class GLShader;
class AssetManager;
class EngineCamera;

class EngineCubeMap
{
protected:
	float upperLimit;
	float lowerLimit;
	uint32_t cubeMap;
	
#ifdef _DEBUG
	GLShader* skyboxShader;
	uPtr<AssetManager> assetManager;
#else
	uPtr<GLShader> skyboxShader;
#endif
	glm::vec3 skycolor;
	GLMesh skyboxMesh;
public:
	EngineCubeMap();
	EngineCubeMap(const std::string& cubeMapDir, const std::string& extension);
	virtual ~EngineCubeMap();
public:
	virtual void updateGUI(void);
	virtual void updateScene(float dt);
	virtual bool initCubeMap(const std::string& cubeMapDir, const std::string& extension);
	virtual void drawScene(const EngineCamera& camera) const = 0;
};

#endif