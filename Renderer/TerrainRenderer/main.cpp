#include <memory>
#include "EngineApp.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "EngineLogger.hpp"

/// for optimus feature enablement, below code must be provided.
extern "C"
{
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

/// Callback function prototypes
static void	error(int error, const char* description);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mousePosCallback(GLFWwindow* window, double xpos, double ypos);
void mouseBtnCallback(GLFWwindow* window, int btn, int action, int mods);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void resizingCallback(GLFWwindow* window, int newWidth, int newHeight);

/// program entry point
std::unique_ptr<EngineApp> gMainApplication;
int main(void)
{
	gMainApplication = std::make_unique<EngineApp>();

	if (!gMainApplication->initEngine())
	{
		EngineLogger::getConsole()->critical("Critical error occurred. program exit.");
		return -1;
	}

	GLFWwindow* window = gMainApplication->getWindow();

	glfwSetErrorCallback(error);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetMouseButtonCallback(window, mouseBtnCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetFramebufferSizeCallback(window, resizingCallback);

	return gMainApplication->Run();
}

void error(int error, const char* description)
{
	gMainApplication->errorCallback(error, description);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	gMainApplication->keyCallback(key, scancode, action, mode);
}

void mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
	gMainApplication->mousePosCallback(xpos, ypos);
}

void mouseBtnCallback(GLFWwindow* window, int btn, int action, int mods)
{
	gMainApplication->mouseBtnCallback(btn, action, mods);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	gMainApplication->scrollCallback(xoffset, yoffset);
}

void resizingCallback(GLFWwindow* window, int newWidth, int newHeight)
{
	gMainApplication->resizingCallback(newWidth, newHeight);
}