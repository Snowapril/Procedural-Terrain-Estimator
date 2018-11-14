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
#include "ShaderCode.hpp"

Generator::Generator()
{
}

Generator::~Generator()
{
}

void Generator::updateScene(void)
{
	noiseGui->updateGUI(guiRect.getRightBottom());

	brushMgr->update(generatorShader);
	
	generatorShader->useProgram();
	generatorShader->sendUniform("presetBlend", noiseGui->getPresetBlend());

	noiseGui->endUpdate(framebuffer->getColorTexture());
}

void Generator::drawScene(void) const
{
	framebuffer->bindFramebuffer(0, 0, 2048, 2048);

	generatorShader->useProgram();
	
	unsigned int texture = noiseGui->getPresetImage();
	if (texture != 0)
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, texture);
	}
	
	brushMgr->bindBrushTextures(0U);
	quadMesh.drawMesh(GL_TRIANGLE_STRIP);
		
	framebuffer->unbindFramebuffer(framebufferRect);

	screenShader->useProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebuffer->getColorTexture());
	quadMesh.drawMesh(GL_TRIANGLE_STRIP);
	
	brushMgr->renderToolUI();

	auto guiRBRect = guiRect.getRightBottom();
	glViewport(0, 0, guiRBRect.x, guiRBRect.y);
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
#ifdef _DEBUG
		generatorShader = std::make_shared<GLShader>(
			"../resources/shader/generator_vs.glsl", 
			"../resources/shader/generator_fs.glsl"
		);
		screenShader = std::make_unique<GLShader>(
			"../resources/shader/offscreen_vs.glsl",
			"../resources/shader/offscreen_fs.glsl"
		);
#else
		generatorShader = std::make_shared<GLShader>();
		screenShader    = std::make_unique<GLShader>();
		generatorShader->loadRawAsset(GENERATOR_VS, GENERATOR_FS);
		screenShader->loadRawAsset(OFFSCREEN_VS, OFFSCREEN_FS);
#endif
	}
	catch (const std::exception& e)
	{
		std::cerr << "Initializing Shader Failed" << std::endl;
		return false;
	}

	generatorShader->useProgram();
	generatorShader->sendUniform("voronoiBoard", 0);
	generatorShader->sendUniform("fbMBoard", 1);
	generatorShader->sendUniform("simplexBoard", 2);
	generatorShader->sendUniform("preset", 3);

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

	quadMesh.initWithFixedShape(MeshShape::QUAD_TRIANGLE_STRIP);

	framebuffer = std::make_unique<GLFramebuffer>();
	framebuffer->initFramebuffer();
	framebuffer->attachColorTexture(2048, 2048, GL_REPEAT, false);

	auto borderLength = clientWidth - min(clientWidth, clientHeight);
	guiRect = Util::Rect(glm::vec2(0.0f, 0.0f), glm::vec2(borderLength, clientHeight));
	framebufferRect = Util::Rect(glm::vec2(borderLength, 0.0f), glm::vec2(clientWidth, clientHeight));

	if (!framebuffer->configureFramebuffer())
		return false;

	if (!initShaders())
		return false;

	brushMgr = std::make_unique<BrushManager>(framebufferRect);

	return true;
}

void Generator::keyCallback(int key, int scancode, int action, int mode)
{
	brushMgr->processToggleKey(key, scancode, action);
}

void Generator::mousePosCallback(double xpos, double ypos)
{
	brushMgr->processCursorPos(xpos, ypos);
}

void Generator::mouseBtnCallback(int btn, int action, int mods)
{
	brushMgr->processMouseBtn(btn, action);
}

void Generator::scrollCallback(double xoffset, double yoffset)
{
	brushMgr->processWheelOffset(yoffset);
}