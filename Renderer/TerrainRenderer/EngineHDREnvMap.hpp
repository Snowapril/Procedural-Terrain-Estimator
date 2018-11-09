/**
* @file		EngineHDREnvMap.hpp
* @author	Shinjihong
* @date		28 September 2018
* @version	1.0.0
* @brief	OpenGL HDR Environment Map Wrapper
* @details	This is for wrapping skybox (for HDR environment map).
* @see		EngineCubeMap
*/

#ifndef ENGINE_HDR_ENVMAP_HPP
#define ENGINE_HDR_ENVMAP_HPP

#include "EngineCubeMap.hpp"

class EngineHDREnvMap : public EngineCubeMap
{
private:
	float rotation;
public:
	EngineHDREnvMap();
	EngineHDREnvMap(const std::string& cubeMapDir, const std::string& extension);
	virtual ~EngineHDREnvMap();
public:
	virtual bool initCubeMap(const std::string& cubeMapDir, const std::string& extension);
	virtual void drawScene(const EngineCamera& camera) const;
	virtual void updateScene(float dt);
};

#endif