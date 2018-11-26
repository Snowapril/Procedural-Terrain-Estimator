#include "BrushManager.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "VoronoiNoise.hpp"
#include "fbMNoise.hpp"
#include "SimplexNoise.hpp"
#include "GLShader.hpp"
#include <algorithm>
#include <iostream>
#include "GLResources.hpp"
#include "ShaderCode.hpp"
#include "obfuscator.hpp"

BrushManager::BrushManager()
	: activeBoardIndex(0), brushMgrMode(BrushManagerMode::NONE), simultaneousApply(false)
{
}

BrushManager::BrushManager(Util::Rect rect)
	: activeBoardIndex(0), brushMgrMode(BrushManagerMode::NONE), simultaneousApply(false)
{
	if (!init(rect)) 
		throw std::exception();
}

BrushManager::~BrushManager()
{
	glDeleteTextures(toolUITextures.size(), &toolUITextures[0]);
}

void BrushManager::configureOrthoMatrix(void)
{
	auto lt = resizedViewport.getLeftTop();
	auto rb = resizedViewport.getRightBottom();
	auto middle = (originalViewport.getRightBottom() + originalViewport.getLeftTop()) * 0.5f;
	auto scale = originalViewport.getScale();

	auto resizedLT = (lt - middle) * 2.0f / scale;
	auto resizedRB = (rb - middle) * 2.0f / scale;

	orthoProject = glm::ortho(resizedLT.x, resizedRB.x, resizedLT.y, resizedRB.y, -1.0f, 1.0f);
}

bool BrushManager::init(Util::Rect rect)
{
	originalViewport = rect;
	resizedViewport = rect;

	configureOrthoMatrix();

	auto voronoi = std::make_shared<VoronoiNoise>(rect);
	auto fbM = std::make_shared<FBMNoise>(rect);
	auto simplex = std::make_shared<SimplexNoise>(rect);

	paintBoards.push_back(voronoi);
	callbackHandler.push_back(voronoi.get());

	paintBoards.push_back(fbM);
	callbackHandler.push_back(fbM.get());

	paintBoards.push_back(simplex);
	callbackHandler.push_back(simplex.get());

	try {
#ifdef _DEBUG
		toolShader = std::make_shared<GLShader>(
			OBFUSCATE("../resources/shader/tool_display_vs.glsl"),
			OBFUSCATE("../resources/shader/tool_display_fs.glsl")
		);
#else
		toolShader = std::make_shared<GLShader>();
		toolShader->loadRawAsset(TOOL_DISPLAY_VS, TOOL_DISPLAY_FS);
#endif
	}
	catch (const std::exception& e) {
		return false;
	}

	toolShader->useProgram();
	toolShader->sendUniform(OBFUSCATE("icon"), 0);
	/*
	toolMeshes[ZOOM_IN] = ...
	toolMeshes[ZOOM_OUT] = ...
	toolMeshes[BRUSH_PLUS] = ...
	toolMeshes[BRUSH_MINUS] = ...
	toolMeshes[LASSO_LINE] = ...
	toolMeshes[BOARD_MOVE] = ...
	toolMeshes[NONE] = ...
	*/
	toolUITextures.resize(7);
	toolUITextures[0] = GLResources::CreateTexture2D(OBFUSCATE("../resources/texture/icons/zoom_in.png"), false);
	toolUITextures[1] = GLResources::CreateTexture2D(OBFUSCATE("../resources/texture/icons/zoom_out.png"), false);
	toolUITextures[2] = GLResources::CreateTexture2D(OBFUSCATE("../resources/texture/icons/move.png"), false);
	toolUITextures[3] = GLResources::CreateTexture2D(OBFUSCATE("../resources/texture/icons/brush.png"), false);
	toolUITextures[4] = GLResources::CreateTexture2D(OBFUSCATE("../resources/texture/icons/brush.png"), false);
	toolUITextures[5] = GLResources::CreateTexture2D(OBFUSCATE("../resources/texture/icons/polygonalLasso.png"), false);
	toolUITextures[6] = GLResources::CreateTexture2D(OBFUSCATE("../resources/texture/icons/freeformLasso.png"), false);

	quadMesh.initWithFixedShape(MeshShape::QUAD_TRIANGLE_STRIP, 0.06f);

	return true;
}

void BrushManager::update(std::shared_ptr<GLShader> shader)
{
	for (auto& board : paintBoards)
	{
		board->updateGUI();
		board->sendProperties(shader);
		board->update(10U);
	}

	shader->useProgram();
	shader->sendUniform(OBFUSCATE("project"), orthoProject);
}

void BrushManager::bindBrushTextures(uint32_t offset) const
{
	for (int i = 0; i < 3; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i + offset);
		glBindTexture(GL_TEXTURE_2D, paintBoards[i]->getBrushTexture());
	}
}

void BrushManager::renderToolUI(void) const
{
	if (brushMgrMode >= toolUITextures.size())
		return;

	toolShader->useProgram();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, toolUITextures[brushMgrMode]);

	glm::vec2 localCursorPos = cursorPos;
	localCursorPos -= originalViewport.getLeftTop();
	localCursorPos /= originalViewport.getScale();
	localCursorPos = (localCursorPos - glm::vec2(0.5f, 0.5f)) * glm::vec2(2.0f, 2.0f);

	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(localCursorPos, 0.0f));
	
	if (brushMgrMode == BrushManagerMode::BRUSH_PLUS || brushMgrMode == BrushManagerMode::BRUSH_MINUS)
	{
		uint32_t radius = paintBoards[activeBoardIndex]->getBrushRadius();
		glm::vec2 scaleFactor(radius, radius);
		scaleFactor /= resizedViewport.getScale() / 6.5f;

		model = glm::scale(model, glm::vec3(scaleFactor, 1.0f));
	}

	toolShader->sendUniform(OBFUSCATE("model"), model);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	quadMesh.drawMesh(GL_TRIANGLE_STRIP);
	glDisable(GL_BLEND);
}

void BrushManager::processCursorPos(double xpos, double ypos)
{
	auto framebufferScale = originalViewport.getScale();
	cursorPos = glm::vec2(xpos, framebufferScale.y - ypos);

	// 400,0 ~ 1200, 800
	// to
	// ex ) 600, 200 ~ 100, 700
	auto resizedLT = resizedViewport.getLeftTop();
	auto originalLT = originalViewport.getLeftTop();
	auto resizedScale = resizedViewport.getScale();
	auto originalScale = originalViewport.getScale();

	glm::vec2 localCursorPos = cursorPos;

	localCursorPos -= originalLT;
	localCursorPos = localCursorPos * resizedScale / originalScale;
	localCursorPos += resizedLT;
	localCursorPos.y = originalScale.y - localCursorPos.y;

	if (simultaneousApply)
	{
		std::for_each(std::begin(callbackHandler), std::end(callbackHandler), [localCursorPos](auto& handler) {
			handler->processCursorPos(localCursorPos.x, localCursorPos.y);
		});
	}
	else
	{
		auto& handler = callbackHandler[activeBoardIndex];
		handler->processCursorPos(localCursorPos.x, localCursorPos.y);
	}
}

void BrushManager::processWheelOffset(double yoffset)
{
	if (simultaneousApply)
	{
		std::for_each(std::begin(callbackHandler), std::end(callbackHandler), [yoffset](auto& handler) {
			handler->processWheelOffset(yoffset);
		});
	}
	else
	{
		auto& handler = callbackHandler[activeBoardIndex];
		handler->processWheelOffset(yoffset);
	}
}

void BrushManager::processMouseBtn(int button, int action)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (brushMgrMode == BrushManagerMode::ZOOM_IN)
		{
			auto reducedScale = resizedViewport.getScale();
			auto originalScale = originalViewport.getScale();
			if (reducedScale.x <= 0.4f * originalScale.x || reducedScale.y <= 0.4f * originalScale.y)
				return;

			reducedScale *= 0.85f;

			auto resizedLT = cursorPos - reducedScale * 0.5f;
			auto resizedRB = cursorPos + reducedScale * 0.5f;
			auto originalLT = originalViewport.getLeftTop();
			auto originalRB = originalViewport.getRightBottom();

			if (resizedLT.x < originalLT.x)
			{
				resizedLT.x = originalLT.x;
				resizedRB.x = originalLT.x + reducedScale.x;
			}

			if (resizedLT.y < originalLT.y)
			{
				resizedLT.y = originalLT.y;
				resizedRB.y = originalLT.y + reducedScale.y;
			}

			if (resizedRB.x > originalRB.x)
			{
				resizedRB.x = originalRB.x;
				resizedLT.x = originalRB.x - reducedScale.x;
			}

			if (resizedRB.y > originalRB.y)
			{
				resizedRB.y = originalRB.y;
				resizedLT.y = originalRB.y - reducedScale.y;
			}

			resizedViewport.setLeftTop(resizedLT);
			resizedViewport.setRightBottom(resizedRB);

			configureOrthoMatrix();
		}

		if (brushMgrMode == BrushManagerMode::ZOOM_OUT)
		{
			auto originalScale = originalViewport.getScale();
			auto reducedScale = resizedViewport.getScale() * 1.5f;
			
			if (reducedScale.x > originalScale.x)
				reducedScale.x = originalScale.x;

			if (reducedScale.y > originalScale.y)
				reducedScale.y = originalScale.y;

			auto middle = (resizedViewport.getLeftTop() + resizedViewport.getRightBottom()) * 0.5f;

			auto resizedLT = middle - reducedScale * 0.5f;
			auto resizedRB = middle + reducedScale * 0.5f;
			auto originalLT = originalViewport.getLeftTop();
			auto originalRB = originalViewport.getRightBottom();

			if (resizedLT.x < originalLT.x)
			{
				resizedLT.x = originalLT.x;
				resizedRB.x = originalLT.x + reducedScale.x;
			}

			if (resizedLT.y < originalLT.y)
			{
				resizedLT.y = originalLT.y;
				resizedRB.y = originalLT.y + reducedScale.y;
			}

			if (resizedRB.x > originalRB.x)
			{
				resizedRB.x = originalRB.x;
				resizedLT.x = originalRB.x - reducedScale.x;
			}

			if (resizedRB.y > originalRB.y)
			{
				resizedRB.y = originalRB.y;
				resizedLT.y = originalRB.y - reducedScale.y;
			}

			resizedViewport.setLeftTop(resizedLT);
			resizedViewport.setRightBottom(resizedRB);

			configureOrthoMatrix();
		}
	}
	
	if (brushMgrMode == BrushManagerMode::ZOOM_IN || brushMgrMode == BrushManagerMode::ZOOM_OUT)
		return;

	if (simultaneousApply)
	{
		std::for_each(std::begin(callbackHandler), std::end(callbackHandler), [button, action](auto& handler) {
			handler->processMouseBtn(button, action);
		});
	}
	else
	{
		auto& handler = callbackHandler[activeBoardIndex];
		handler->processMouseBtn(button, action);
	}
}

void BrushManager::processToggleKey(int key, int scancode, int action)
{
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
		simultaneousApply = false;

	if (key >= GLFW_KEY_1 && key <= GLFW_KEY_3)
	{
		activeBoardIndex = key - GLFW_KEY_1;
	}

	if (simultaneousApply)
	{
		std::for_each(std::begin(callbackHandler), std::end(callbackHandler), [key, scancode, action](auto& handler) {
			handler->processToggleKey(key, scancode, action);
		});
	}
	else
	{
		auto& handler = callbackHandler[activeBoardIndex];
		handler->processToggleKey(key, scancode, action);
	}

	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_Z:
			brushMgrMode = BrushManagerMode::ZOOM_IN;
			break;
		case GLFW_KEY_X:
			brushMgrMode = BrushManagerMode::ZOOM_OUT;
			break;
		default:
			brushMgrMode = static_cast<BrushManagerMode>(paintBoards[activeBoardIndex]->getCurrentMode());
			break;
		}
	}
}
