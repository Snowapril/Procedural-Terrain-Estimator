#ifndef GOD_RAYS_HPP
#define GOD_RAYS_HPP

#include <memory>
#include "GLFramebuffer.hpp"
#include "GLMesh.hpp"
#include <vector>
#include <glm/vec2.hpp>

class AssetManager;
class GLShader;
class EngineCamera;
class LightSourceWrapper;

class Godrays
{
private:
	float spacing;

	GLShader* flareShader;
    GLShader* effectShader;

    std::unique_ptr<GLFramebuffer> screenBuffer;
    std::unique_ptr<AssetManager> assetManager;

	std::vector<uint32_t> flareTextures;

	GLMesh flareMesh;
public:
    Godrays();
    ~Godrays();
public:
	bool initLensFlare(float spacing, size_t numTextures);
    bool initGodrays(int width, int height);

    inline void bindGodrayBuffer(int width, int height) const
    {
        screenBuffer->bindFramebuffer(width, height);
    }

    inline void unbindGodrayBuffer(int width, int height) const 
    {
        screenBuffer->unbindFramebuffer(width, height);
    }
	
	void drawLensFlare(const EngineCamera& camera, const LightSourceWrapper& lightSource, const glm::vec2& centerScreen) const;
    void drawGodrays(void) const ;
};

#endif
