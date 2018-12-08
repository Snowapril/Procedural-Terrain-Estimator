#ifndef BRUSH_UI_HPP
#define BRUSH_UI_HPP

#include "BrushBoard.hpp"
#include "EventHandler.hpp"
#include "GLMesh.hpp"
#include <array>

enum BrushManagerMode {
	ZOOM_IN = 0,
	ZOOM_OUT = 1,
	BOARD_MOVE = 2,
	BRUSH_PLUS = 3,
	BRUSH_MINUS = 4,
	LASSO_LINE = 5,
	LASSO_FREE = 6,
	CLEAR = 7,
	NONE = 8,
};

class BrushManager : public EventHandler
{
private:
	BrushManagerMode brushMgrMode;
	glm::mat4 orthoProject;

	Util::Rect originalViewport;
	Util::Rect resizedViewport;
	
	GLMesh quadMesh;
	std::vector<uint32_t> toolUITextures;

	std::vector<std::shared_ptr<BrushBoard>> paintBoards;
	std::vector<EventHandler*> callbackHandler;
	std::shared_ptr<GLShader> toolShader;
	std::unique_ptr<GLShader> toolLineShader;
	glm::vec2 cursorPos;
	uint32_t activeBoardIndex;
	uint32_t lineSegmentsVAO;
	uint32_t lineSegmentsVBO;

	bool simultaneousApply;
public:
	BrushManager();
	BrushManager(Util::Rect rect);
	~BrushManager(); 
private:
	void configureOrthoMatrix(void);
public:
	bool init(Util::Rect rect);

#ifdef _DEBUG
	glm::mat4 const& getOrthoMatrix(void) const
	{
		return orthoProject;
	}
#endif
	inline Util::Rect getResizedViewport(void) const
	{
		return resizedViewport;
	}

	inline BrushManagerMode getBrushManagerMode(void) const
	{
		return brushMgrMode;
	}

	void update(std::shared_ptr<GLShader> shader);
	void bindBrushTextures(uint32_t offset) const;
	void renderToolUI(void) const;

	void processCursorPos(double xpos, double ypos);
	void processWheelOffset(double yoffset);
	void processMouseBtn(int button, int action);
	void processToggleKey(int key, int scancode, int action);
};

#endif