#ifndef BRUSH_BOARD_HPP
#define BRUSH_BOARD_HPP

#include <stdint.h>
#include <glm/vec2.hpp>
#include <vector>
#include <functional>
#include <deque>
#include "EventHandler.hpp"

enum class BrushMode : uint32_t {
	PLUS  = 0,
	MINUS = 1,
	CUT   = 2,
	MOVE  = 3,
	CLEAR = 4,
	NONE  = 5,
};

class BrushBoard : public EventHandler
{
private:
	bool isScreenClicked;

	uint32_t counter;
	uint32_t clientWidth;
	uint32_t clientHeight;
	uint32_t width;
	uint32_t height;
	uint32_t brushTexture;

	glm::vec2 viewPoint;
	
	BrushMode paintMode;

	std::function<void(double, double)> paintFuncPtr;

	std::vector<float> brush;
	std::vector<float> board;
	std::deque<std::pair<float, float>> cutPoints;

	struct BoolWrapper {
		bool b;
	};
	std::vector<BoolWrapper> visitPixels;
public:
	BrushBoard();
	BrushBoard(uint32_t clientWidth, uint32_t clientHeight, uint32_t width, uint32_t height);
	~BrushBoard();
private:
	void writePaint(double xpos, double ypos);
	void erasePaint(double xpos, double ypos);
	void setCutPoint(double xpos, double ypos);
	void moveViewPoint(double xpos, double ypos);

	void drawLine(std::pair<float, float> start, std::pair<float, float> end);
	void dfsFloodFill(uint32_t x, uint32_t y);

	inline bool isInRange(uint32_t x, uint32_t y) {
		return x >= 0 && x < width && y >= 0 && y <= height;
	}
	inline static size_t ConvertToIndex(size_t x, size_t y, size_t width) {
		return x + y * width;
	}
public:
	bool initBrushBoard(uint32_t clientWidth, uint32_t clientHeight, uint32_t width, uint32_t height);

	void processCursorPos(double xpos, double ypos) override;
	void processWheelOffset(double yoffset) override;
	void processMouseBtn(int button, int action) override;
	void processToggleKey(int key, int scancode, int action) override;

	void updateBrushTexture(uint32_t cycle);

	inline uint32_t getBrushTexture(void) const {
		return brushTexture;
	}

	inline glm::vec2 getViewPoint(void) const {
		return this->viewPoint;
	}
	
	void setPaintMode(BrushMode mode);
	void setBoardSize(uint32_t width, uint32_t height);
	glm::uvec2 getBoardSize(void) const;
};

#endif