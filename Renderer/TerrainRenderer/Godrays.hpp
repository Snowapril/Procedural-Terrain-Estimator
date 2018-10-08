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

template <typename T>
using uPtr = std::unique_ptr<T>;

class Godrays
{
private:
    uPtr<GLFramebuffer> screenBuffer;
public:
    Godrays();
    ~Godrays();
public:
    bool initGodrays(int width, int height);

#ifdef _DEBUG

#endif

    inline void bindGodrayBuffer(int width, int height) const
    {
        screenBuffer->bindFramebuffer(width, height);
    }

    inline void unbindGodrayBuffer(int width, int height) const 
    {
        screenBuffer->unbindFramebuffer(width, height);
    }
	
	inline uint32_t getColorTexture(void) const
	{
		return screenBuffer->getColorTexture();
	}
};

#endif
