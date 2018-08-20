#ifndef GL_APP_HPP
#define GL_APP_HPP

#include "EngineTimer.hpp"
#include <string>

struct GLFWwindow;

class GLApp
{
protected:
	GLFWwindow * window;
	int clientWidth;
	int clientHeight;
	bool fullscreen;
	bool paused;
	std::string WndCaption;

	EngineTimer timer;
protected:
	bool initGLWindow(bool fullscreen);
	bool initGLConfigure(void);

	void calculateFrameStats(void);

	virtual void onResize(int newWidth, int newHeight);
	virtual void updateScene(float dt) = 0;
	virtual void drawScene(void) const = 0;
public:
	GLApp();
	~GLApp();
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
	void errorCallback(int error, const char* description) const;
	virtual void keyCallback(int key, int scancode, int action, int mode) = 0;
	virtual void mousePosCallback(double xpos, double ypos) = 0;
	virtual void mouseBtnCallback(int btn, int action, int mods) = 0;
	virtual void scrollCallback(double xoffset, double yoffset) = 0;
	void resizingCallback(int newWidth, int newHeight);
};

#endif