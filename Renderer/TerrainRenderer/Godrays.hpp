#ifndef GOD_RAYS_HPP
#define GOD_RAYS_HPP

#include <memory>

class GLFramebuffer;
class AssetManager;
class GLShader;

class Godrays
{
private:
    GLShader* effectShader;

    std::unique_ptr<GLFramebuffer> screenBuffer;
    std::unique_ptr<AssetManager> assetManager;
public:
    Godrays();
    ~Godrays();
public:
    bool initGodrays(int width, int height);
    inline void bindGodrayBuffer(int width, int height) const
    {
        screenBuffer->bindFramebuffer(width, height);
    }

    inline void unbindGodrayBuffer(int width, int height) const 
    {
        screenBuffer->unbindFramebuffer(width, height);
    }

    void drawScene(void) const ;
};

#endif
