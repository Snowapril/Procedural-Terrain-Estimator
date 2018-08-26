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
#include <memory>

class GLShader;
class AssetManager;

class EngineApp : public GLApp
{
private:
	GLShader* simpleShader; /// for test
	unsigned int VAO; /// for test
	std::unique_ptr<AssetManager> assetManager; /// for hot reload test.
protected:
	bool initShader(void);
	bool initTextures(void);
	bool initAssets(void);
	bool initGeometryBuffer(void);

	void updateScene(float dt);
	void drawScene(void) const;
public:
	EngineApp();
	virtual ~EngineApp();
public:
	/// at the start of program, this method must be called first.
	virtual bool init(void);

	/// callback functions below.
	void keyCallback(int key, int scancode, int action, int mode);
	void mousePosCallback(double xpos, double ypos);
	void mouseBtnCallback(int btn, int action, int mods);
	void scrollCallback(double xoffset, double yoffset);
};

#endif