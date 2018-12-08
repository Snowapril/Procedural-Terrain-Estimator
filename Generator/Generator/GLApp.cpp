#include "GLApp.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "EngineProperty.hpp"
#include "GLDebugger.hpp"
#include <sstream>	
#include <iostream>
#include "obfuscator.hpp"

GLApp::GLApp()
	: window(nullptr), clientWidth(0), clientHeight(0), fullscreen(FULL_SCREEN), paused(false), WndCaption(OBFUSCATE("Generator & Estimator"))
{
}

GLApp::~GLApp()
{
	glfwTerminate();
}

/**
* @ brief		initialize OpenGL window with GLFW.
* @ details		initialize OpenGL with version 4.3 . check full screen enabled, if so, turn on the application with fullscreen.
				finally log user's hardware information (vendor, renderer).
* @ param		if true, window will initialized to fullscreen mode.
* @ return		return boolean whether if initializing OpenGL is successful or not.
*/
bool GLApp::initGLWindow(bool fullscreen)
{
	if (!glfwInit())
	{
		std::cerr << OBFUSCATE("GLFW Initialization failed") << std::endl;
		return false;
	}

	/// use opengl version 4.3 (tessellation is enabled after opengl 4.0)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if (ENABLE_4XMSAA) glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
#ifdef __APPLE__ /// this is enabled when apple computer run this program.
	glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GLFW_OPENGL_FORWARD_COMPAT);
#endif

	GLFWmonitor* glfwMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* glfwMode = glfwGetVideoMode(glfwMonitor);

	glfwWindowHint(GLFW_RED_BITS, glfwMode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, glfwMode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, glfwMode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, glfwMode->refreshRate);

	this->fullscreen = fullscreen;
	clientWidth = fullscreen ? glfwMode->width : CLIENT_WIDTH;
	clientHeight = fullscreen ? glfwMode->height : CLIENT_HEIGHT;

	window = glfwCreateWindow(clientWidth, clientHeight, WndCaption.c_str(), fullscreen ? glfwMonitor : nullptr, nullptr);

	if (!window)
	{
		glfwTerminate();
		std::cerr << OBFUSCATE("GLFW Window Creating failed.") << std::endl;
		return false;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		std::cerr << OBFUSCATE("GLAD loading opengl functions failed.") << std::endl;
		return false;
	}

	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);

	std::clog << OBFUSCATE("Vendor : ") << vendor << OBFUSCATE(", Renderer : ") << renderer << std::endl;

	return true;
}

/**
* @ brief		OpenGL Configuration like enabling depth test or back culling.
* @ details		enable culling, 4x multi sample, depthtest and etc according to EngineProperty which defined in "EngineProperty.hpp"
				later, will be moved to json configure file.
* @ return		return true always. just for consistency.
*/
bool GLApp::initGLConfigure(void)
{
	if (ENABLE_CULLING)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}
	glLineWidth(2.0f);
	glfwSwapInterval(0);

	return true;
}

/**
* @ brief		deal with whole opengl application initialization.
* @ return		return boolean whether if intialize opengl is successful or not.
*/
bool GLApp::initGLApp(void)
{
	if (!initGLWindow(FULL_SCREEN))
		return false;
	if (!initGLConfigure())
		return false;

	return true;
}

GLFWwindow * GLApp::getWindow(void) const
{
	return window;
}

/**
* @ brief		this is main loop for opengl application.
* @ details		first, reset the timer. in main loop, timer always tick. if user pause the application, system will fall in sleep.
				when get out from loop, Check error mainly about invalud enumeration and Log application performace.
* @ return		this will be connected to system exit code.
*/
int GLApp::Run(void)
{
	while (!glfwWindowShouldClose(window))
	{
		if (!paused)
		{
			updateScene();
			drawScene();
		}
		else
		{
			Sleep(100);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	CheckError();

	return EXIT_SUCCESS;
}

/**
* @ brief		this method will be called whenever framebuffer size is changed.
*/
void GLApp::onResize(int newWidth, int newHeight)
{
	clientWidth = newWidth;
	clientHeight = newHeight;

	glViewport(0, 0, clientWidth, clientHeight);
}

void GLApp::resizingCallback(int newWidth, int newHeight)
{
	if (newWidth == 0) newWidth = 1;
	if (newHeight == 0) newHeight = 1;

	onResize(newWidth, newHeight);
}