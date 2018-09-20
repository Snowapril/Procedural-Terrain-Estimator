#include "EngineApp.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "EngineProperty.hpp"
#include "EngineProfiler.hpp"
#include "EngineLogger.hpp"
#include "GLShader.hpp"
#include "AssetManager.hpp"
#include <glm/gtc/matrix_transform.hpp>

EngineApp::EngineApp()
	: GLApp(), polygonMode(GL_FILL), camera(glm::vec3(300.0f, 100.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f))
{
}

EngineApp::~EngineApp()
{
}

/**
* @ brief		update terrain renderer.
* @ details		this will be located in main loop. update whole terrain renderer like position, direction, status and etc...
*/
void EngineApp::updateScene(float dt)
{
	Profile();

	processKeyInput(dt);
	camera.onUpdate(dt);

	camera.sendVP(vpUBO, GLApp::getAspectRatio());

	const glm::vec3 cameraPos = camera.getViewPos();

	terrain.updateScene(dt, cameraPos);
	
	//EngineGUI::updateGUI(dt, clientHeight);
}

/**
* @ brief		render opengl world!
* @ details		render opengl world using shaders, other objects and etc ...
*/
void EngineApp::drawScene(void) const
{
	Profile();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(Color::Black[0], Color::Black[1], Color::Black[2], Color::Black[3]);

	const float totalTime = timer.getTotalTime();

	glBindBuffer(GL_UNIFORM_BUFFER, vpUBO);
	/// draw call here.
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	terrain.drawScene(GL_PATCHES);
	skybox.drawScene(GL_TRIANGLES);

	/// end of draw call
	glBindVertexArray(0u);
	glBindTexture(GL_TEXTURE_2D, 0u);
	glBindBuffer(GL_UNIFORM_BUFFER, 0u);

	//EngineGUI::renderGUI();
}

/**
* @ brief		deal with whole initialization of Terrain Renderer.
* @ return		return boolean whether if intializing terrain renderer is successful or not.
*/
bool EngineApp::initEngine(void)
{
	if (!GLApp::initGLApp())
		return false;

	if (!EngineGUI::initGUI(window))
		return false;

	if (!initUniformBufferObject())
		return false;

	if (!terrain.initTerrain(glm::vec3(0.0f, 0.0f, 0.0f), {}))
		return false;

	onResize(clientWidth, clientHeight);

	if (!skybox.initSkybox("../resources/texture/skybox/interstella/", "png"))
		return false;

	if (!water.initWater(REFLECTION_WIDTH, REFLECTION_HEIGHT, REFRACTION_WIDTH, REFRACTION_HEIGHT))
		return false;
	
	water.setTransform(glm::vec3(0.0f), glm::vec3(clientWidth, clientHeight, 1.0f));
	
	return true;
}

/**
* @ brief		initialize uniform buffer object
* @ details		uniform buffer object is useful when many shader use common uniform variable.
* @ return		return boolean whether if initializing uniform buffer object is successful or not.
*/
bool EngineApp::initUniformBufferObject(void)
{
	glGenBuffers(1, &vpUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, vpUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, vpUBO, 0, sizeof(glm::mat4) * 2);

	return true;
}

void EngineApp::keyCallback(int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
		polygonMode = polygonMode == GL_LINE ? GL_FILL : GL_LINE;
}

void EngineApp::mousePosCallback(double xpos, double ypos)
{
	camera.processMousePos(xpos, ypos);
}

void EngineApp::mouseBtnCallback(int btn, int action, int mods)
{
	unsigned int keyFlag = 0;

	if (btn == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		keyFlag |= CAMERA_LEFT_BTN;

	if (btn == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		keyFlag |= CAMERA_RIGHT_BTN;

	camera.processMouseBtn(keyFlag);
}

void EngineApp::scrollCallback(double xoffset, double yoffset)
{
	camera.processScroll(yoffset);
}

void EngineApp::processKeyInput(float dt)
{
	unsigned int keyFlag = 0;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		keyFlag |= CAMERA_UP;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		keyFlag |= CAMERA_LEFT;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		keyFlag |= CAMERA_DOWN;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		keyFlag |= CAMERA_RIGHT;

	camera.processKeyInput(keyFlag, dt);
}