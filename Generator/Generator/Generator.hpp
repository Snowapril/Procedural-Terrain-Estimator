#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "GLApp.hpp"

class Generator : public GLApp
{
private:
	bool isGUIOpen;
protected:
	void updateScene(void);
	void drawScene(void) const;
	void onResize(int newWidth, int newHeight);

	bool initGUI(void);
	void updateGUI(float height);
public:
	Generator();
	virtual ~Generator();
public:
	bool initGenerator(void);

	void keyCallback(int key, int scancode, int action, int mode);
	void mousePosCallback(double xpos, double ypos);
	void mouseBtnCallback(int btn, int action, int mods);
	void scrollCallback(double xoffset, double yoffset);
};

#endif