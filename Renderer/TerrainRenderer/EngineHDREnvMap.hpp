#ifndef ENGINE_HDR_ENVMAP_HPP
#define ENGINE_HDR_ENVMAP_HPP

#include "EngineCubeMap.hpp"

class EngineHDREnvMap : public EngineCubeMap
{
public:
	EngineHDREnvMap();
	EngineHDREnvMap(const std::string& cubeMapDir, const std::string& extension);
	virtual ~EngineHDREnvMap();
public:
	virtual bool initCubeMap(const std::string& cubeMapDir, const std::string& extension);
	virtual void drawScene(const EngineCamera& camera) const;
};

#endif