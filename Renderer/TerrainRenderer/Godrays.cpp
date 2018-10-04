#include "Godrays.hpp"
#include "EngineLogger.hpp"

Godrays::Godrays()
{

}

Godrays::~Godrays()
{

}

bool initGodrays(int width, int height)
{
    assetManager = std::make_unique<AssetManager>();

    try
    {
        effectShader = assetManager->addAsset<GLShader, std::string>({
            "../resources/shader/godrays_vs.glsl",
            "../resources/shader/godrays_fs.glsl",
        });
    }
    catch (std::exception e) 
    {
        EngineLogger::getConsole()->error("Godrays initialization failed");
        return false; 
    }

    screenBuffer = std::make_unique<GLFramebuffer>();

    screenBuffer->initFramebuffer();
    screenBuffer->attachDepthTexture(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
    
    if (!screenBuffer->configureFramebuffer())
        return false;

    return true;
}

void Godrays::drawScene(void) const 
{

}
