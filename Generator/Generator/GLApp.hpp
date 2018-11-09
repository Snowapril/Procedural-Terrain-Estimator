/**
* @file		GLApp.hpp
* @author	Shinjihong
* @date		27 August 2018
* @version	1.0.0
* @brief	this represent opengl renderer. GLApp manage opengl features which are needed for Renderer.
* @see
*/

#ifndef GL_APP_HPP
#define GL_APP_HPP

#include <string>
#include "NonCopyable.hpp"

struct GLFWwindow;

class GLApp : public NonCopyable
{
protected:
	std::string WndCaption;
	GLFWwindow * window;
	int clientHeight;
	int clientWidth;
	bool fullscreen;
	bool paused;
protected:
	bool initGLWindow(bool fullscreen);
	bool initGLConfigure(void);

	virtual void updateScene(void) = 0;
	virtual void drawScene(void) const = 0;
	virtual void onResize(int newWidth, int newHeight);
public:
	GLApp();
	virtual ~GLApp();
public:
	bool initGLApp(void);

	/// this return opengl window. if you want to set callback function or etc .. , get window from this method.
	GLFWwindow* getWindow(void) const;
	inline float getAspectRatio(void) const
	{
		return static_cast<float>(clientWidth) / static_cast<float>(clientHeight);
	};

	/// this run main loop of terrain renderer and finally return exit code.
	int Run(void);

	//callback functions below.
	virtual void keyCallback(int key, int scancode, int action, int mode) = 0;
	virtual void mousePosCallback(double xpos, double ypos) = 0;
	virtual void mouseBtnCallback(int btn, int action, int mods) = 0;
	virtual void scrollCallback(double xoffset, double yoffset) = 0;
	void resizingCallback(int newWidth, int newHeight);
};

#endif