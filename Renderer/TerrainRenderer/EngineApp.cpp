#include "EngineApp.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "EngineProperty.hpp"
#include "EngineLogger.hpp"
#include "GLShader.hpp"
#include "AssetManager.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "EngineSkybox.hpp"
#include "Util.hpp"
#include <imgui/imgui.h>

#ifdef _DEBUG
#include <NvToolsExt/nvToolsExt.h>
#define NVTX_PUSH(label) nvtxRangePushA((label))
#define NVTX_POP() nvtxRangePop()
#else
#define NVTX_PUSH(label)
#define NVTX_POP() 
#endif

#include "obfuscator.hpp"

EngineApp::EngineApp()
	: debuggerMode(false), enableVsync(false), polygonMode(GL_FILL)
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
	NVTX_PUSH("Update");

	if (enableVsync)
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);

	processKeyInput(dt);
	camera.onUpdate(dt);
	camera.updateProject(getAspectRatio());

	updateGUI(dt);

	NVTX_POP();
}

void EngineApp::updateGUI(float dt)
{
	GUI.startUpdateGUI(dt, clientHeight);

	terrain.updateGUI(water);
	water.updateGUI();
	postprocess.updateGUI();
	skybox->updateGUI();
	camera.updateGUI();
	lightWrapper.updateGUI();

	if (ImGui::CollapsingHeader("Rendering Property"))
	{
		ImGui::Checkbox("Debugger Mode", &debuggerMode);
		ImGui::Checkbox("V-sync", &enableVsync);
	}

	GUI.endUpdateGUI(dt);
}

/**
* @ brief		render opengl world!
* @ details		render opengl world using shaders, other objects and etc ...
*/
void EngineApp::drawScene(void)
{
	const float totalTime = timer.getTotalTime();
	const float waterHeight = water.getWaterHeight();
	const float ratio = getAspectRatio();

	/// draw call here.
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	const glm::vec3 cameraPos = camera.getViewPos();
	const float dt = timer.getDeltaTime();

	NVTX_PUSH("Frame");
	terrain.updateScene(dt, water, camera);
	skybox->updateScene(dt);

	NVTX_PUSH("Gen-Water-Textures");
	NVTX_PUSH("Gen-Reflection");
	water.bindReflectionFramebuffer(clientWidth, clientHeight);
	glEnable(GL_CLIP_DISTANCE0);
	glDisable(GL_CULL_FACE);

	camera.flipVertically(waterHeight);

	terrain.drawScene(camera, lightWrapper, glm::vec4(0.0f, 1.0f, 0.0f, -waterHeight + 2.0f));
	skybox->drawScene(camera);

	camera.flipVertically(waterHeight);
	NVTX_POP();

	NVTX_PUSH("Gen-Refraction");
	water.bindRefractionFramebuffer(clientWidth, clientHeight);
	terrain.drawScene(camera, lightWrapper, glm::vec4(0.0f, -1.0f, 0.0f, waterHeight));
	//skybox->drawScene(camera);

	water.unbindCurrentFramebuffer(clientWidth, clientHeight);
	NVTX_POP();
	NVTX_POP();

	glDisable(GL_CLIP_DISTANCE0);
	glEnable(GL_CULL_FACE);

	// depth pass here.

	NVTX_PUSH("Gen-Shadow-Map");
	const glm::vec3& scale = terrain.getTerrainScale();
	lightWrapper.bindDepthPassBuffer(scale.x, scale.z);

	terrain.drawScene_DepthPass(camera, lightWrapper, false, glm::vec4(0.0f, -1.0f, 0.0f, 15000.0f));

	lightWrapper.unbindDepthPassBuffer(clientWidth, clientHeight);
	NVTX_POP();
	// depth pass end

	NVTX_PUSH("Light-Occlude");
	godray.bindGodrayBuffer(clientWidth, clientHeight);
	lightWrapper.renderSun(camera);
	terrain.drawScene_DepthPass(camera, lightWrapper, true, glm::vec4(0.0f, -1.0f, 0.0f, 15000.0f));
	godray.unbindGodrayBuffer(clientWidth, clientHeight);
	NVTX_POP();

	NVTX_PUSH("Main-Render");
	water.updateWater(dt);
	postprocess.bindPostprocessing(clientWidth, clientHeight);

	//glDepthMask(GL_FALSE);
	//glBeginQuery(GL_ANY_SAMPLES_PASSED, testQuery);
	NVTX_PUSH("Terrain-Render");
	terrain.drawScene(camera, lightWrapper, glm::vec4(0.0f, -1.0f, 0.0f, 15000.0f));
	NVTX_POP();
	//glEndQuery(GL_ANY_SAMPLES_PASSED);
	//glDepthMask(GL_TRUE);

	NVTX_PUSH("Skybox-Render");
	skybox->drawScene(camera);
	NVTX_POP();
	NVTX_PUSH("Water-Render");
	water.drawWater(camera, lightWrapper);
	NVTX_POP();
	NVTX_PUSH("Sun-Render");
	lightWrapper.renderSun(camera);
	NVTX_POP();

	postprocess.unbindPostprocessing(clientWidth, clientHeight);
	NVTX_POP();


	NVTX_PUSH("Post-processing");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	postprocess.updateScene(dt);
	postprocess.drawScene(camera, lightWrapper, godray.getColorTexture());
	NVTX_POP();

	if (debuggerMode)
	{
		textureViewer.addTextureView(glm::vec2(0.8f, 0.8f), glm::vec2(0.15f, 0.15f), water.getReflectionTexture());
		textureViewer.addTextureView(glm::vec2(0.8f, 0.4f), glm::vec2(0.15f, 0.15f), water.getRefractionTexture());
		textureViewer.addDepthTextureView(glm::vec2(0.8f, 0.0f), glm::vec2(0.15f, 0.15f), postprocess.getOffscreenDepthTexture());
		textureViewer.renderViewer(camera.getMinDepth(), camera.getMaxDepth());
		textureViewer.clearViewer();
	}

	/// end of draw call
	glBindVertexArray(0u);
	glBindTexture(GL_TEXTURE_2D, 0u);

	NVTX_PUSH("GUI");
	GUI.renderGUI();
	NVTX_POP();

	NVTX_POP();
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
		skybox = std::make_unique<EngineSkybox>(OBFUSCATE("../resources/texture/skybox/cloud/"), OBFUSCATE("jpg"));
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
	camera.initCamera(glm::vec3(0.0f, terrainScale.y * 1.5f, terrainScale.z * 0.3f), glm::vec3(0.0f, -0.2f, -0.5f), terrain.getTerrainScale());
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

	const glm::vec3 sumPosition(0.0f, 800.0f, 10000.0f);
	lightWrapper.addDirLight(-sumPosition, glm::vec3(1.0f, 0.85f, 0.72f) * 1.7f);

	if (!godray.initGodrays(clientWidth, clientHeight))
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

	uint32_t keyFlag = 0;

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		keyFlag |= CAMERA_1;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		keyFlag |= CAMERA_2;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		keyFlag |= CAMERA_3;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		keyFlag |= CAMERA_4;
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		keyFlag |= CAMERA_5;

	camera.processKeyCallback(keyFlag);

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
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