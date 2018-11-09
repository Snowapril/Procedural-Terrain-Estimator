/**
* @file		EngineSkybox.hpp
* @author	Shinjihong
* @date		28 September 2018
* @version	1.0.0
* @brief	OpenGL Skybox Wrapper
* @details	Wrapping Skybox textures and provide update and render method.
* @see		EngineCubeMap
*/

#ifndef ENGINE_SKYBOX_HPP
#define ENGINE_SKYBOX_HPP

#include "EngineCubeMap.hpp"

class EngineSkybox : public EngineCubeMap
{
private:
	float rotation;
	float speed;

public:
	EngineSkybox();
	EngineSkybox(const std::string& cubeMapDir, const std::string& extension);
	virtual ~EngineSkybox();
public:
	virtual bool initCubeMap(const std::string& cubeMapDir, const std::string& extension);
	virtual void drawScene(const EngineCamera& camera) const;
	virtual void updateScene(float dt);
	virtual void updateGUI(void);
};

#endif