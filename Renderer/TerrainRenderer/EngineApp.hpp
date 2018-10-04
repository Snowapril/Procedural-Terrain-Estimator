/**
* @file		EngineApp.hpp
* @author	Shinjihong
* @date		27 August 2018
* @version	1.0.0
* @brief	this representate Renderer itself. provide only initialization method and input method to public.
* @see		GLApp
*/

#ifndef ENGINE_APP_HPP
#define ENGINE_APP_HPP

#include "GLApp.hpp"
#include "EngineGUI.hpp"
#include <memory>
#include "EngineTerrain.hpp"
#include "EngineCamera.hpp"
#include "EngineCubeMap.hpp"
#include "EngineWater.hpp"
#include "TextureViewer.hpp"
#include "LightSourceWrapper.hpp"
#include "Godrays.hpp"

class GLShader;
class AssetManager;
class GLTexture;

class EngineApp : public GLApp
{
private:
	bool debuggerMode;

	uint32_t vpUBO;
	uint32_t polygonMode;

	GLShader* hdrShader;
	
	std::unique_ptr<EngineCubeMap> skybox;
	std::unique_ptr<AssetManager> assetManager;

	GLMesh framebufferMesh;
	GLFramebuffer hdrFramebuffer;
	LightSourceWrapper lightWrapper;
	TextureViewer textureViewer;
	Godrays rayEffect;
	EngineCamera camera;
	EngineWater water;
	EngineTerrain terrain;

	EngineGUI GUI;
protected:
	bool initUniformBufferObject(void);
	bool initAssets(void);

	void updateScene(float dt);
	void drawScene(void) ;
public:
	EngineApp();
	virtual ~EngineApp();
public:
	/// at the start of program, this method must be called first.
	bool initEngine(void);

	/// callback functions below.
	void processKeyInput(float dt);
	void keyCallback(int key, int scancode, int action, int mode);
	void mousePosCallback(double xpos, double ypos);
	void mouseBtnCallback(int btn, int action, int mods);
	void scrollCallback(double xoffset, double yoffset);

};

#endif