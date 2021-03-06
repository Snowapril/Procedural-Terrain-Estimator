#include "Generator.hpp"
#include <memory>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "obfuscator.hpp"

/// Callback function prototypes
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mousePosCallback(GLFWwindow* window, double xpos, double ypos);
void mouseBtnCallback(GLFWwindow* window, int btn, int action, int mods);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void resizingCallback(GLFWwindow* window, int newWidth, int newHeight);

/// program entry point
std::unique_ptr<Generator> gMainApplication;
int main(void)
{
	gMainApplication = std::make_unique<Generator>();

	if (!gMainApplication->initGenerator())
	{
		std::cerr << OBFUSCATE("Critical error occurred. program exit.") << std::endl;
		return EXIT_FAILURE;
	}

	GLFWwindow* window = gMainApplication->getWindow();

	/// register callback functions to window which is current context.
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetMouseButtonCallback(window, mouseBtnCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetFramebufferSizeCallback(window, resizingCallback);

	return gMainApplication->Run();
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