#ifndef ENGINE_APP_HPP
#define ENGINE_APP_HPP

#include "GLApp.hpp"

class EngineApp : public GLApp
{
private:
protected:
	bool initShader(void);
public:
	EngineApp();
	virtual ~EngineApp();
public:
	bool initEngine(void);

	void updateScene(float dt);
	void drawScene(void) const;

	//callback functions below.
	void keyCallback(int key, int scancode, int action, int mode);
	void mousePosCallback(double xpos, double ypos);
	void mouseBtnCallback(int btn, int action, int mods);
	void scrollCallback(double xoffset, double yoffset);
};

#endif