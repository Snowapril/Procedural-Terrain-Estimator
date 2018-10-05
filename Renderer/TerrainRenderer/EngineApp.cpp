#include "EngineApp.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "EngineProperty.hpp"
#include "EngineProfiler.hpp"
#include "EngineLogger.hpp"
#include "GLShader.hpp"
#include "AssetManager.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "EngineSkybox.hpp"
#include "Util.hpp"

EngineApp::EngineApp()
	: GLApp(), debuggerMode(false), polygonMode(GL_FILL), camera(glm::vec3(300.0f, 300.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f))
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

	camera.updateProject(getAspectRatio());

	const glm::vec3 cameraPos = camera.getViewPos();

	terrain.updateScene(dt, cameraPos);
	water.updateWater(dt);
	
	skybox->updateScene(dt);

	postprocess.updateScene(dt);
	
	//GUI.updateGUI(dt, clientHeight);
}

/**
* @ brief		render opengl world!
* @ details		render opengl world using shaders, other objects and etc ...
*/
void EngineApp::drawScene(void) 
{
	Profile();
	glClearColor(Color::Black[0], Color::Black[1], Color::Black[2], Color::Black[3]);

	const float totalTime = timer.getTotalTime();
	const float waterHeight = water.getWaterHeight();
	const float ratio = getAspectRatio();

	/// draw call here.
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	water.bindReflectionFramebuffer(clientWidth, clientHeight);
		glEnable(GL_CLIP_DISTANCE0);
		glDisable(GL_CULL_FACE);

		camera.flipVertically(waterHeight);
		
		terrain.drawScene(camera, lightWrapper, glm::vec4(0.0f, 1.0f, 0.0f, -waterHeight + 2.0f));
		skybox->drawScene(camera);
		
		camera.flipVertically(waterHeight);

	water.bindRefractionFramebuffer(clientWidth, clientHeight);
		terrain.drawScene(camera, lightWrapper, glm::vec4(0.0f, -1.0f, 0.0f, waterHeight));
		//skybox->drawScene(camera);
	
		water.unbindCurrentFramebuffer(clientWidth, clientHeight);

	glDisable(GL_CLIP_DISTANCE0);
	glEnable(GL_CULL_FACE);

	// depth pass here.

	const glm::vec3& scale = terrain.getTerrainScale();
	lightWrapper.bindDepthPassBuffer(scale.x, scale.z);
		glCullFace(GL_FRONT);

		terrain.drawScene_DepthPass(camera, lightWrapper, glm::vec4(0.0f, -1.0f, 0.0f, 15000.0f));

		glCullFace(GL_BACK);
		lightWrapper.unbindDepthPassBuffer(clientWidth, clientHeight);
	// depth pass end

	postprocess.bindPostprocessing(clientWidth, clientHeight);

		//glDepthMask(GL_FALSE);
		//glBeginQuery(GL_ANY_SAMPLES_PASSED, testQuery);
		terrain.drawScene(camera, lightWrapper, glm::vec4(0.0f, -1.0f, 0.0f, 15000.0f));
		//glEndQuery(GL_ANY_SAMPLES_PASSED);
		//glDepthMask(GL_TRUE);

		skybox->drawScene(camera);
		water.drawWater(camera, lightWrapper);
		lightWrapper.renderSun(camera);
		rayEffect.drawLensFlare(camera, lightWrapper, glm::vec2(0.0f));

		postprocess.unbindPostprocessing(clientWidth, clientHeight);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	postprocess.drawScene(camera);
	   
	if (debuggerMode)
	{
		textureViewer.addTextureView(glm::vec2(0.8f, 0.8f), glm::vec2(0.15f, 0.15f), water.getReflectionTexture());
		textureViewer.addTextureView(glm::vec2(0.8f, 0.4f), glm::vec2(0.15f, 0.15f), water.getRefractionTexture());
		textureViewer.renderViewer(camera.getMinDepth(), camera.getMaxDepth());
		textureViewer.clearViewer();
	}

	/// end of draw call
	glBindVertexArray(0u);
	glBindTexture(GL_TEXTURE_2D, 0u);

	//GUI.renderGUI();
}

/**
* @ brief		deal with whole initialization of Terrain Renderer.
* @ return		return boolean whether if intializing terrain renderer is successful or not.
*/
bool EngineApp::initEngine(void)
{
	if (!GLApp::initGLApp())
		return false;

	if (!GUI.initGUI(window))
		return false;

	if (!textureViewer.initTextureViewer())
		return false;

	if (!terrain.initTerrain(glm::vec3(0.0f, 0.0f, 0.0f), {}))
		return false;

	onResize(clientWidth, clientHeight);

	try
	{
		skybox = std::make_unique<EngineSkybox>("../resources/texture/skybox/cloud/", "jpg");
	}
	catch (std::exception e)
	{
		return false;
	}

	onResize(clientWidth, clientHeight);

	//glGenQueries(1, &testQuery);

	if (!water.initWater(REFLECTION_WIDTH, REFLECTION_HEIGHT, REFRACTION_WIDTH, REFRACTION_HEIGHT))
		return false;	
	
	glm::vec3 terrainScale = terrain.getTerrainScale() * 0.5f;
	water.setTransform(glm::vec3(0.0f, terrainScale.y * 0.5f, 0.0f), glm::vec3(terrainScale.x, 1.0f, terrainScale.z));
	
	if (!postprocess.initPostProcessing(clientWidth, clientHeight))
		return false;

	if (!initAssets())
		return false;
	
	return true;
}

bool EngineApp::initAssets(void)
{
	if (!lightWrapper.initDepthPassBuffer(SHADOW_RESOLUTION_X, SHADOW_RESOLUTION_Y))
		return false;

	const glm::vec3 sumPosition(0.0f, 800.0f, 5000.0f);
	lightWrapper.addDirLight(-sumPosition, glm::vec3(1.0f, 0.85f, 0.72f) * 1.7f);

	if (!rayEffect.initLensFlare(0.1f, 9))
		return false;

	return true;
}

void EngineApp::onResize(int newWidth, int newHeight)
{
	GLApp::onResize(newWidth, newHeight);

	camera.setViewportSize(newWidth, newHeight);
}

void EngineApp::keyCallback(int key, int scancode, int action, int mode)
{
	GLApp::keyCallback(key, scancode, action, mode);

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
		polygonMode = polygonMode == GL_LINE ? GL_FILL : GL_LINE;

	if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
		debuggerMode = !debuggerMode;

	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		camera.processKeyCallback(CAMERA_AUTO);
}

void EngineApp::mousePosCallback(double xpos, double ypos)
{
	camera.processMousePos(xpos, ypos);
}

void EngineApp::mouseBtnCallback(int btn, int action, int mods)
{
	uint32_t keyFlag = 0;

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
	uint32_t keyFlag = 0;

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