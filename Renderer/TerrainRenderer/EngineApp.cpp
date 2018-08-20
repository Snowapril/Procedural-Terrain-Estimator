#include "EngineApp.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "EngineProperty.hpp"
#include "EngineProfiler.hpp"

EngineApp::EngineApp()
	: GLApp()
{
}

EngineApp::~EngineApp()
{
}

/**
* @ brief		deal with whole initialization of Terrain Renderer.
* @ return		return boolean whether if intializing terrain renderer is successful or not.
*/
bool EngineApp::initEngine(void)
{
	if (!GLApp::initGLApp())
		return false;

	if (!initShader())
		return false;

	return true;
}

/** 
* @ brief		initialize shaders.
* @ return		return boolean whether if initializing shader is successful or not.
*/
bool EngineApp::initShader(void)
{
	return true;
}

/**
* @ brief		update terrain renderer.
* @ details		this will be located in main loop. update whole terrain renderer like position, direction, status and etc...
*/
void EngineApp::updateScene(float dt)
{
	Profile();
}

/**
* @ brief		render opengl world!
* @ details		render opengl world using shaders, other objects and etc ... 
*/
void EngineApp::drawScene(void) const
{
	Profile();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(Color::LightSteelBlue[0], Color::LightSteelBlue[1], Color::LightSteelBlue[2], Color::LightSteelBlue[3]);


}

void EngineApp::keyCallback(int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void EngineApp::mousePosCallback(double xpos, double ypos)
{
}

void EngineApp::mouseBtnCallback(int btn, int action, int mods)
{
}

void EngineApp::scrollCallback(double xoffset, double yoffset)
{
}