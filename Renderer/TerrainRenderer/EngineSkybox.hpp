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