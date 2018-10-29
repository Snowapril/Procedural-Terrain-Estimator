#include "Generator.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "EngineProperty.hpp"
#include <vector>
#include "GLDebugger.hpp"
#include "GLShader.hpp"
#include <iostream>
#include "BrushBoard.hpp"
#include "NoiseGUI.hpp"
#include "EventHandler.hpp"
#include "GLFramebuffer.hpp"

Generator::Generator()
	: activeBoardIndex(1)
{
}

Generator::~Generator()
{
}

void Generator::updateScene(void)
{
	noiseGui->updateGUI(static_cast<float>(clientHeight), framebuffer->getColorTexture(), activeBoardIndex);

	noiseGui->sendProperties(generatorShader);

	for (size_t i = 0; i < 3; ++i)
	{
		paintBoards[i]->updateBrushTexture(10U);	
	}
}

void Generator::drawScene(void) const
{
	framebuffer->bindFramebuffer(0, 0, 2048, 2048);

	generatorShader->useProgram();
	for (int i = 0; i < 3; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, paintBoards[i]->getBrushTexture());
	}

	quadMesh.drawMesh(GL_TRIANGLE_STRIP);
		
	framebuffer->unbindFramebuffer(400, 0, clientWidth - 400, clientHeight);

	screenShader->useProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebuffer->getColorTexture());
	quadMesh.drawMesh(GL_TRIANGLE_STRIP);

	glViewport(0, 0, 400, clientHeight);
	noiseGui->renderGUI();
}

void Generator::onResize(int newWidth, int newHeight)
{
	GLApp::onResize(newWidth, newHeight);
}

bool Generator::initShaders(void)
{
	try
	{
		generatorShader = std::make_unique<GLShader>(
			"../resources/shader/generator_vs.glsl", 
			"../resources/shader/generator_fs.glsl"
		);
		screenShader = std::make_unique<GLShader>(
			"../resources/shader/offscreen_vs.glsl",
			"../resources/shader/offscreen_fs.glsl"
		);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Initializing Shader Failed" << std::endl;
		return false;
	}

	generatorShader->useProgram();
	generatorShader->sendUniform("voronoiBoard", 0);
	generatorShader->sendUniform("simplexBoard", 1);
	generatorShader->sendUniform("fbMBoard", 2);

	screenShader->useProgram();
	screenShader->sendUniform("framebufferTexture", 0);

	return true;
}

bool Generator::initFramebuffer(int width, int height)
{
	quadMesh.initWithFixedShape(MeshShape::QUAD_TRIANGLE_STRIP);

	framebuffer = std::make_unique<GLFramebuffer>();
	framebuffer->initFramebuffer();
	framebuffer->attachColorTexture(2048, 2048, GL_REPEAT, false);
	
	if (!framebuffer->configureFramebuffer())
		return false;

	return true;
}

bool Generator::initGenerator(void)
{
	if (!GLApp::initGLApp())
		return false;

	noiseGui = std::make_unique<NoiseGUI>();
	if (!noiseGui->initGUI(window))
		return false;

	callbackHandler.push_back(noiseGui.get());

	quadMesh.initWithFixedShape(MeshShape::QUAD_TRIANGLE_STRIP);

	framebuffer = std::make_unique<GLFramebuffer>();
	framebuffer->initFramebuffer();
	framebuffer->attachColorTexture(2048, 2048, GL_REPEAT, false);

	if (!framebuffer->configureFramebuffer())
		return false;

	if (!initShaders())
		return false;

	for (size_t i = 0; i < 3; ++i)
	{
		paintBoards[i] = std::make_shared<BrushBoard>(clientWidth, clientHeight, 2048, 2048);
		callbackHandler.push_back(paintBoards[i].get());
	}

	return true;
}

void Generator::keyCallback(int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= GLFW_KEY_1 && key <= GLFW_KEY_3) 
	{
		activeBoardIndex = key - GLFW_KEY_1 + 1;
	}

	auto& handler = callbackHandler[activeBoardIndex];
	handler->processToggleKey(key, scancode, action);
}

void Generator::mousePosCallback(double xpos, double ypos)
{
	auto& handler = callbackHandler[activeBoardIndex];
	handler->processCursorPos(xpos, ypos);
}

void Generator::mouseBtnCallback(int btn, int action, int mods)
{
	auto& handler = callbackHandler[activeBoardIndex];
	handler->processMouseBtn(btn, action);
}

void Generator::scrollCallback(double xoffset, double yoffset)
{
	auto& handler = callbackHandler[activeBoardIndex];
	handler->processWheelOffset(yoffset);
}