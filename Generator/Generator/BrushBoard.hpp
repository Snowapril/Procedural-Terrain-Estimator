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
	CLEAR = 3,
	NONE  = 4,
};

class BrushBoard : public EventHandler
{
private:
	bool isScreenClicked;

	uint32_t counter;
	uint32_t width;
	uint32_t height;
	uint32_t brushTexture;
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
	BrushBoard(uint32_t width, uint32_t height);
	~BrushBoard();
private:
	void writePaint(double xpos, double ypos);
	void erasePaint(double xpos, double ypos);
	void setCutPoint(double xpos, double ypos);

	inline size_t convertToIndex(size_t x, size_t y) {
		return x + y * width;
	}
public:
	bool initBrushBoard(uint32_t width, uint32_t height);

	void processCursorPos(double xpos, double ypos) override;
	void processWheelOffset(double yoffset) override;
	void processMouseBtn(int button, int action) override;
	void processToggleKey(int key, int scancode, int action) override;

	void updateBrushTexture(uint32_t cycle);

	inline uint32_t getBrushTexture(void) const {
		return brushTexture;
	}
	
	void setPaintMode(BrushMode mode);
	void setBoardSize(uint32_t width, uint32_t height);
	glm::uvec2 getBoardSize(void) const;
};

#endif