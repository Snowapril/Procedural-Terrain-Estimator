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
#include "EngineSkybox.hpp"

class GLShader;
class AssetManager;
class GLTexture;

class EngineApp : public GLApp, EngineGUI
{
private:
	unsigned int vpUBO;

	EngineSkybox skybox;
	EngineCamera camera;
	EngineTerrain terrain;
protected:
	bool initShader(void);
	bool initTextures(void);
	bool initAssets(void);
	bool initGeometryBuffer(void);
	bool initUniformBufferObject(void);

	void updateScene(float dt);
	void drawScene(void) const;
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