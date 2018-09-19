#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "GLApp.hpp"

class Generator : public GLApp
{
private:
	bool isGUIOpen;

	unsigned int VAO;
	unsigned int VBO;
	unsigned int framebuffer;
	unsigned int framebufferTexture;
protected:
	void updateScene(void);
	void drawScene(void) const;
	void onResize(int newWidth, int newHeight);

	bool initGUI(void);
	void updateGUI(float height);

	bool saveCurrentTexture(const std::string& path);
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