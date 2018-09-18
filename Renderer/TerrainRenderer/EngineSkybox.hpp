#ifndef ENGINE_SKYBOX_HPP
#define ENGINE_SKYBOX_HPP

#include "GLMesh.hpp"
#include <memory>
#include <string>

class GLShader;
class AssetManager;

class EngineSkybox 
{
private:
	unsigned int cubeMap;
    GLShader* skyboxShader;
    std::unique_ptr<AssetManager> assetManager;
    GLMesh skyboxMesh;
public:
	EngineSkybox();
	~EngineSkybox();
public:
    bool initSkybox(const std::string& skyboxDir, const std::string& extension);
    void drawScene(unsigned int drawMode) const;
};

#endif