#ifndef BRUSH_BOARD_HPP
#define BRUSH_BOARD_HPP

#include <stdint.h>
#include <glm/vec2.hpp>
#include <vector>
#include <functional>
#include "EventHandler.hpp"
#include "Util.hpp"
#include <array>
#include "EngineProperty.hpp"
#include <deque>

class GLShader;

enum class BrushMode : uint32_t {
	MOVE = 2,
	PLUS = 3,
	MINUS	 = 4,
	CUT		 = 5,
	FREEFORM = 6,
	CLEAR	 = 7,
	NONE	 = 8,
};

class BrushBoard : public EventHandler
{
private:
	Util::Rect boardRect;
	std::vector<std::vector<unsigned char>> brush;
	std::vector<glm::vec2> cutPoints;
	std::array<std::array<unsigned char, BOARD_WIDTH>, BOARD_HEIGHT> board;
	std::array<std::array<bool, BOARD_WIDTH>, BOARD_HEIGHT> visitPixels;
	std::array<std::array<bool, BOARD_WIDTH>, BOARD_HEIGHT> floodPixels;

	struct BoardStorage {
		std::array<std::array<unsigned char, BOARD_WIDTH>, BOARD_HEIGHT> board;
		glm::vec2 viewPoint;
	};

	std::deque<BoardStorage> actionMemory;

	std::function<void(double, double)> paintFuncPtr;
	BrushMode paintMode;
	glm::vec2 viewPoint;
	uint32_t brushTexture;
	int brushRadius;
	uint32_t counter;
	bool isScreenClicked;
public:
	BrushBoard();
	BrushBoard(const Util::Rect& rect);
	virtual ~BrushBoard();
private:
	void writePaint(double xpos, double ypos);
	void erasePaint(double xpos, double ypos);
	void setCutPoint(double xpos, double ypos);
	void drawCutPoint(double xpos, double ypos);
	void moveViewPoint(double xpos, double ypos);

	void drawLineLow(glm::vec2 start, glm::vec2 end);
	void drawLineHigh(glm::vec2 start, glm::vec2 end);
	void drawLine(glm::vec2 start, glm::vec2 end);
	void dfsFloodFill(void);

	void writeWorker(int xpos, int ypos, int offsetY);
	void eraseWorker(int xpos, int ypos, int offsetY);

	inline bool isInRange(uint32_t x, uint32_t y) {
		return x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT;
	}
	inline static size_t ConvertToIndex(size_t x, size_t y, size_t width) {
		return x + y * width;
	}
public:
	bool initBrushBoard(const Util::Rect& rect);

	void processCursorPos(double xpos, double ypos) override;
	void processWheelOffset(double yoffset) override;
	void processMouseBtn(int button, int action) override;
	void processToggleKey(int key, int scancode, int action) override;

	void changeBrushRadius(int newRadius);

	virtual void updateGUI(void) = 0;
	virtual void sendProperties(std::shared_ptr<GLShader> shader) = 0;

	void update(uint32_t cycle);

	inline uint32_t getBrushTexture(void) const {
		return brushTexture;
	}

	inline glm::vec2 getViewPoint(void) const {
		return this->viewPoint;
	}
	
	inline uint32_t getBrushRadius(void) const {
		return brushRadius;
	}

	inline BrushMode getCurrentMode(void) const {
		return paintMode;
	}

	void setBoardRect(Util::Rect rect);
	void setPaintMode(BrushMode mode);
	glm::uvec2 getBoardSize(void) const;
};

#endif