#include "BrushBoard.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>
#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <queue>
#include "obfuscator.hpp"
#include <limits>

BrushBoard::BrushBoard()
	: isScreenClicked(false), counter(10U), brushTexture(0U), paintFuncPtr(nullptr), paintMode(BrushMode::NONE), viewPoint(0.0f, 0.0f)
{
}

BrushBoard::BrushBoard(const Util::Rect& rect)
	: isScreenClicked(false), counter(10U), brushTexture(0U), paintFuncPtr(nullptr), paintMode(BrushMode::NONE), viewPoint(0.0f, 0.0f)
{
	if (!initBrushBoard(rect))
		throw std::exception(OBFUSCATE("Failed to initialize brush board"));
}

BrushBoard::~BrushBoard()
{
	// BUG : access viloation here why?
	// if (brushTexture) 
	// 	glDeleteTextures(GL_TEXTURE_2D, &brushTexture);
}

bool BrushBoard::initBrushBoard(const Util::Rect& rect)
{
	this->boardRect = rect;

	for (auto& row : board)
		std::fill(row.begin(), row.end(), 65535);

	for (auto& rowPixels : visitPixels)
		std::fill(rowPixels.begin(), rowPixels.end(), false);

	for (auto& rowPixels : floodPixels)
		std::fill(rowPixels.begin(), rowPixels.end(), false);

	glGenTextures(1, &brushTexture);
	glBindTexture(GL_TEXTURE_2D, brushTexture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, BOARD_WIDTH, BOARD_HEIGHT, 0, GL_RED, GL_UNSIGNED_SHORT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	changeBrushRadius(BRUSH_RADIUS_LIST[BRUSH_DEFAULT_RADIUS_IDX]);

	return true;
}

void BrushBoard::update(uint32_t cycle)
{
	if (counter >= cycle)
	{
		glBindTexture(GL_TEXTURE_2D, brushTexture);	
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, BOARD_WIDTH, BOARD_HEIGHT, GL_RED, GL_UNSIGNED_SHORT, &board[0][0]);
		glBindTexture(GL_TEXTURE_2D, 0);
		counter = 0;
	}

	++counter;
}

void BrushBoard::processCursorPos(double xpos, double ypos)
{
	static glm::dvec2 previousPoint(xpos, ypos);

	if (paintMode == BrushMode::NONE || previousPoint == glm::dvec2(xpos, ypos) || !boardRect.isInBound(xpos, ypos))
		return;

	auto leftTop = boardRect.getLeftTop();
	auto rightBottom = boardRect.getRightBottom();

	xpos = (xpos - leftTop.x) * BOARD_WIDTH / (rightBottom.x - leftTop.x);
	ypos = (-ypos + rightBottom.y) * BOARD_HEIGHT / (rightBottom.y - leftTop.y);

	paintFuncPtr(xpos, ypos);
}

void BrushBoard::processWheelOffset(double yoffset)
{

}

void BrushBoard::processMouseBtn(int button, int action)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) 
		isScreenClicked = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		isScreenClicked = false;
}

void BrushBoard::processToggleKey(int key, int scancode, int action)
{
	if (action == GLFW_PRESS)
	{
		switch (key) 
		{
		case GLFW_KEY_B : 
			setPaintMode(BrushMode::PLUS);
			break;
		case GLFW_KEY_N :
			setPaintMode(BrushMode::MINUS);
			break;
		case GLFW_KEY_C : 
			setPaintMode(BrushMode::CUT);
			break;
		case GLFW_KEY_F :
			setPaintMode(BrushMode::FREEFORM);
			break;
		case GLFW_KEY_M : 
			setPaintMode(BrushMode::MOVE);
			break;
		case GLFW_KEY_R:
			setPaintMode(BrushMode::CLEAR);
			break;
		case GLFW_KEY_LEFT_BRACKET:
			if (brushRadius != BRUSH_RADIUS_LIST[0]) {
				auto iter = std::find(BRUSH_RADIUS_LIST.begin(), BRUSH_RADIUS_LIST.end(), brushRadius);
				changeBrushRadius(*(iter - 1));
			}
			break;
		case GLFW_KEY_RIGHT_BRACKET:
			if (brushRadius != BRUSH_RADIUS_LIST.back()) {
				auto iter = std::find(BRUSH_RADIUS_LIST.begin(), BRUSH_RADIUS_LIST.end(), brushRadius);
				changeBrushRadius(*(iter + 1));
			}
		default :
			break;
		}
	}
}

void BrushBoard::changeBrushRadius(int newRadius)
{
	this->brushRadius = newRadius;

	brush = std::vector<std::vector<unsigned short>>(brushRadius, std::vector<unsigned short>(brushRadius));

	constexpr unsigned short maxValue = ~0;
	const int radius = brushRadius / 2;

	const int centerX = brushRadius / 2;
	const int centerY = brushRadius / 2;

	for (int r = 0; r < brushRadius; ++r)
	{
		for (int c = 0; c < brushRadius; ++c)
		{
			float distance = std::sqrt((centerX - r) * (centerX - r) + (centerY - c) * (centerY - c));
			float radian = glm::degrees(distance * glm::pi<float>() * 0.5f / radius);

			brush[r][c] = distance > radius ? 0 : glm::cos(radian) * maxValue;
		}
	}
}

void BrushBoard::setBoardRect(Util::Rect rect)
{
	boardRect = rect;
}

void BrushBoard::setPaintMode(BrushMode mode)
{
	using namespace std::placeholders;

	this->paintMode = mode;

	switch (this->paintMode) 
	{
	case BrushMode::PLUS :
		paintFuncPtr = std::bind(&BrushBoard::writePaint, this, _1, _2);
		break;
	case BrushMode::MINUS : 
		paintFuncPtr = std::bind(&BrushBoard::erasePaint, this, _1, _2);
		break;
	case BrushMode::CUT :
		paintFuncPtr = std::bind(&BrushBoard::setCutPoint, this, _1, _2);
		break;
	case BrushMode::FREEFORM :
		paintFuncPtr = std::bind(&BrushBoard::drawCutPoint, this, _1, _2);
		break;	
	case BrushMode::MOVE :
		paintFuncPtr = std::bind(&BrushBoard::moveViewPoint, this, _1, _2);
		break;
	case BrushMode::CLEAR:
		for (auto& row : board)
			std::fill(row.begin(), row.end(), 255);
		for (auto& rowPixels : floodPixels)
			std::fill(rowPixels.begin(), rowPixels.end(), false);
		for (auto& rowPixels : visitPixels)
			std::fill(rowPixels.begin(), rowPixels.end(), false);
		this->paintMode = BrushMode::NONE;
		cutPoints.clear();
		break;
	case BrushMode::NONE:
	default :
		break;
	}
}

glm::uvec2 BrushBoard::getBoardSize(void) const
{
	return glm::uvec2(BOARD_WIDTH, BOARD_HEIGHT);
}

void BrushBoard::writeWorker(int xpos, int ypos, int offsetY)
{
	const int halfSize = brushRadius / 2;
	const int assignedWork = brushRadius / NUM_WORKER;
	
	for (int y = offsetY; y < offsetY + assignedWork; ++y)
		for (int x = 0; x < brushRadius; ++x) {
			if (isInRange(x - halfSize + xpos, y - halfSize + ypos)) {
				if (65535 - this->board[y - halfSize + ypos][x - halfSize + xpos] >= this->brush[y][x])
					this->board[y - halfSize + ypos][x - halfSize + xpos] += this->brush[y][x];
				else
					this->board[y - halfSize + ypos][x - halfSize + xpos] = 65535;
			}
		}
}

void BrushBoard::writePaint(double xpos, double ypos)
{
	if (!isScreenClicked)
		return;
	
	const int ixpos = static_cast<int>(xpos);
	const int iypos = static_cast<int>(ypos);

	std::array<std::thread, NUM_WORKER> workers;

	const int assignedWork = brushRadius / NUM_WORKER;
	for (int i = 0; i < NUM_WORKER; ++i)
		workers[i] = std::thread(&BrushBoard::writeWorker, this, ixpos, iypos, i * assignedWork);

	for (auto& worker : workers)
		worker.join();
}

void BrushBoard::eraseWorker(int xpos, int ypos, int offsetY)
{
	const int halfSize = brushRadius / 2;

	const int assignedWork = brushRadius / NUM_WORKER;
	for (int y = offsetY; y < offsetY + assignedWork; ++y)
		for (int x = 0; x < brushRadius; ++x) {
			if (isInRange(x - halfSize + xpos, y - halfSize + ypos)) {
				if (this->board[y - halfSize + ypos][x - halfSize + xpos] >= this->brush[y][x])
					this->board[y - halfSize + ypos][x - halfSize + xpos] -= this->brush[y][x];
				else
					this->board[y - halfSize + ypos][x - halfSize + xpos] = 0;
			}
		}
}

void BrushBoard::erasePaint(double xpos, double ypos)
{
	if (!isScreenClicked)
		return;

	const int ixpos = static_cast<int>(xpos);
	const int iypos = static_cast<int>(ypos);

	std::array<std::thread, NUM_WORKER> workers;

	const int assignedWork = brushRadius / NUM_WORKER;
	for (int i = 0; i < NUM_WORKER; ++i)
		workers[i] = std::thread(&BrushBoard::eraseWorker, this, ixpos, iypos, i * assignedWork);

	for (auto& worker : workers)
		worker.join();
}

void BrushBoard::moveViewPoint(double xpos, double ypos)
{
	static bool isFirstUse = true;

	if (!isScreenClicked)
	{
		isFirstUse = true;
		return;
	}

	static glm::vec2 previousPoint(xpos, ypos);
	if (isFirstUse)
		previousPoint = glm::vec2(xpos, ypos);

	isFirstUse = false;

	constexpr float sensitivity = 0.003f;
	const glm::vec2 currentPoint(xpos, ypos);

	const glm::vec2 diff = previousPoint - currentPoint;

	this->viewPoint += sensitivity * diff;

	previousPoint = currentPoint;
}

void BrushBoard::drawCutPoint(double xpos, double ypos)
{
	if (!isScreenClicked)
		return;

	if (cutPoints.empty())
		cutPoints.push_back(glm::vec2(xpos, ypos));
	else
		if (cutPoints.size() > 15 && glm::length(cutPoints.front() - glm::vec2(xpos, ypos)) <= 20.0f)
		{
			drawLine(cutPoints.back(), cutPoints.front());
			dfsFloodFill();

			for (auto& rowPixels : floodPixels)
				std::fill(rowPixels.begin(), rowPixels.end(), false);
			for (auto& rowPixels : visitPixels)
				std::fill(rowPixels.begin(), rowPixels.end(), false);

			cutPoints.clear();
		}
		else
		{
			drawLine(cutPoints.back(), glm::vec2(xpos, ypos));
			cutPoints.push_back(glm::vec2(xpos, ypos));
		}
}

void BrushBoard::setCutPoint(double xpos, double ypos)
{
	if (!isScreenClicked)
		return;
	isScreenClicked = false;

	if (cutPoints.empty())
		cutPoints.push_back(glm::vec2(xpos, ypos));
	else
		if (glm::length(cutPoints.front() - glm::vec2(xpos, ypos)) <= 30.0f)
		{
			drawLine(cutPoints.back(), cutPoints.front());
			dfsFloodFill();

			for (auto& rowPixels : floodPixels)
				std::fill(rowPixels.begin(), rowPixels.end(), false);
			for (auto& rowPixels : visitPixels)
				std::fill(rowPixels.begin(), rowPixels.end(), false);

			cutPoints.clear();
		}
		else
		{
			drawLine(cutPoints.back(), glm::vec2(xpos, ypos));
			cutPoints.push_back(glm::vec2(xpos, ypos));
		}
}

void BrushBoard::drawLineLow(glm::vec2 start, glm::vec2 end)
{
	float dx = end.x - start.x;
	float dy = end.y - start.y;
	int yi = dy < 0 ? -1 : 1;
	dy = dy < 0 ? -dy : dy;

	float D = 2 * dy - dx;
	int y = start.y;
	for (int x = start.x; x <= static_cast<int>(end.x); ++x)
	{
		visitPixels[y][x] = true;
		if (D > 0)
		{
			y += yi;
			D -= 2 * dx;
		}
		
		D += 2 * dy;
	}
}

void BrushBoard::drawLineHigh(glm::vec2 start, glm::vec2 end)
{
	float dx = end.x - start.x;
	float dy = end.y - start.y;
	int xi = dx < 0 ? -1 : 1;
	dx = dx < 0 ? -dx : dx;

	float D = 2 * dx - dy;
	int x = start.x;

	for (int y = start.y; y <= static_cast<int>(end.y); ++y)
	{
		visitPixels[y][x] = true;
		if (D > 0)
		{
			x += xi;
			D -= 2 * dy;
		}

		D += 2 * dx;
	}
}

void BrushBoard::drawLine(glm::vec2 start, glm::vec2 end)
{
	if (std::abs(end.y - start.y) < abs(end.x - start.x))
		if (start.x > end.x)
			drawLineLow(end, start);
		else
			drawLineLow(start, end);
	else
		if (start.y > end.y)
			drawLineHigh(end, start);
		else
			drawLineHigh(start, end);
}

void BrushBoard::dfsFloodFill(void)
{
	std::queue<glm::uvec2> pixelStack;
	pixelStack.push(glm::uvec2(0U, 0U));

	while (!pixelStack.empty()) 
	{
		auto pixel = pixelStack.front();
		pixelStack.pop();

		board[pixel.y][pixel.x] = 0;

		if (pixel.x < BOARD_WIDTH - 1 && !floodPixels[pixel.y][pixel.x + 1] && !visitPixels[pixel.y][pixel.x + 1])
		{
			pixelStack.push(glm::uvec2(pixel.x + 1, pixel.y));
			floodPixels[pixel.y][pixel.x + 1] = true;
		}
		if (pixel.x > 0 && !floodPixels[pixel.y][pixel.x - 1] && !visitPixels[pixel.y][pixel.x - 1])
		{
			pixelStack.push(glm::uvec2(pixel.x - 1, pixel.y));
			floodPixels[pixel.y][pixel.x - 1] = true;
		}
		if (pixel.y > 0 && !floodPixels[pixel.y - 1][pixel.x] && !visitPixels[pixel.y - 1][pixel.x])
		{
			pixelStack.push(glm::uvec2(pixel.x, pixel.y - 1));
			floodPixels[pixel.y - 1][pixel.x] = true;
		}
		if (pixel.y < BOARD_HEIGHT - 1 && !floodPixels[pixel.y + 1][pixel.x] && !visitPixels[pixel.y + 1][pixel.x])
		{
			pixelStack.push(glm::uvec2(pixel.x, pixel.y + 1));
			floodPixels[pixel.y + 1][pixel.x] = true;
		}
	}
}