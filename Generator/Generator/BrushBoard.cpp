#include "BrushBoard.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

constexpr uint32_t  BRUSH_WIDTH		  = 100;
constexpr uint32_t  BRUSH_HEIGHT	  = 100;
constexpr uint32_t  HALF_BRUSH_WIDTH  = BRUSH_WIDTH / 2;
constexpr uint32_t  HALF_BRUSH_HEIGHT = BRUSH_HEIGHT / 2;
constexpr uint32_t	COUNTER_HUDDLE	  = 10;
constexpr float		BRUSH_DECAL		  = 0.002f;

BrushBoard::BrushBoard()
	: isScreenClicked(false), clientWidth(0U), clientHeight(0U), counter(0U), width(0U), height(0U), brushTexture(0U), paintFuncPtr(nullptr), paintMode(BrushMode::NONE)
{
}

BrushBoard::BrushBoard(uint32_t clientWidth, uint32_t clientHeight, uint32_t width, uint32_t height)
	: isScreenClicked(false), clientWidth(0U), clientHeight(0U), counter(0U), width(0U), height(0U), brushTexture(0U), paintFuncPtr(nullptr), paintMode(BrushMode::NONE)
{
	if (!initBrushBoard(clientWidth, clientHeight, width, height))
		throw std::exception("Failed to initialize brush board");
}

BrushBoard::~BrushBoard()
{
	if (brushTexture)
		glDeleteTextures(GL_TEXTURE_2D, &brushTexture);
}

bool BrushBoard::initBrushBoard(uint32_t clientWidth, uint32_t clientHeight, uint32_t width, uint32_t height)
{
	this->clientWidth = clientWidth;
	this->clientHeight = clientHeight;
	this->width = width;
	this->height = height;

	brush.resize(BRUSH_WIDTH * BRUSH_HEIGHT);
	board.resize(this->width * this->height, 1.0f);
	visitPixels.resize(this->width * this->height, { false });

	glGenTextures(1, &brushTexture);
	glBindTexture(GL_TEXTURE_2D, brushTexture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, this->width, this->height, 0, GL_RED, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
	
	constexpr int centerX = BRUSH_WIDTH / 2;
	constexpr int centerY = BRUSH_HEIGHT / 2;
	
	for (int r = 0; r < BRUSH_WIDTH; ++r)
	{
		for (int c = 0; c < BRUSH_HEIGHT; ++c)
		{
			float distance = std::sqrt((centerX - r) * (centerX - r) + (centerY - c) * (centerY - c));

			brush[r * BRUSH_HEIGHT + c] = std::exp(-distance * distance * BRUSH_DECAL);
		}
	}

	return true;
}

void BrushBoard::updateBrushTexture(uint32_t cycle)
{
	if (counter >= cycle)
	{
		glBindTexture(GL_TEXTURE_2D, brushTexture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_FLOAT, &board[0]);
		glBindTexture(GL_TEXTURE_2D, 0);
		counter = 0;
	}

	++counter;
}

void BrushBoard::processCursorPos(double xpos, double ypos)
{
	if (paintMode == BrushMode::NONE)
		return;

	//0 ~ clientWidth, clientHeight ~ 0
	// ->
	//0 ~ width, 0 ~ height
	
	xpos = xpos * width / clientWidth;
	ypos = (-ypos + clientHeight) * height / clientHeight;

	paintFuncPtr(xpos, ypos);
}

void BrushBoard::processWheelOffset(double yoffset)
{

}

void BrushBoard::processMouseBtn(int button, int action)
{
	using namespace std::placeholders;

	if (button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT)
		isScreenClicked = true;
	else
		isScreenClicked = false;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) 
	{
		paintMode = BrushMode::PLUS;
		paintFuncPtr = std::bind(&BrushBoard::writePaint, this, _1, _2);
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		paintMode = BrushMode::NONE;	
		paintFuncPtr = nullptr;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) 
	{
		paintMode = BrushMode::MINUS;
		paintFuncPtr = std::bind(&BrushBoard::erasePaint, this, _1, _2);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		paintMode = BrushMode::NONE;
		paintFuncPtr = nullptr;
	}
}

void BrushBoard::processToggleKey(int key, int scancode, int action)
{
	if (action == GLFW_PRESS)
	{
		switch (key) 
		{
		case GLFW_KEY_X : 
			setPaintMode(BrushMode::CUT);
			break;
		case GLFW_KEY_M : 
			setPaintMode(BrushMode::MOVE);
			break;
		default :
			break;
		}
	}
}

void BrushBoard::setPaintMode(BrushMode mode)
{
	using namespace std::placeholders;

	this->paintMode = mode;

	switch (this->paintMode) 
	{
	case BrushMode::PLUS :
		break;
	case BrushMode::MINUS : 
		break;
	case BrushMode::CUT :
		paintFuncPtr = std::bind(&BrushBoard::setCutPoint, this, _1, _2);
		break;
	case BrushMode::MOVE :
		paintFuncPtr = std::bind(&BrushBoard::moveViewPoint, this, _1, _2);
		break;
	case BrushMode::CLEAR:
		std::fill(board.begin(), board.end(), 1.0f);
		break;
	case BrushMode::NONE:
	default :
		break;
	}
}

void BrushBoard::setBoardSize(uint32_t width, uint32_t height)
{
	this->width  = width;
	this->height = height;
}

glm::uvec2 BrushBoard::getBoardSize(void) const
{
	return glm::uvec2(width, height);
}

void BrushBoard::writePaint(double xpos, double ypos)
{
	//xpos = 0 ~ width, ypos = 0 ~ height;
	uint32_t uxpos = static_cast<uint32_t>(xpos);
	uint32_t uypos = static_cast<uint32_t>(ypos);

	for (uint32_t x = 0; x < BRUSH_WIDTH; ++x)
		for (uint32_t y = 0; y < BRUSH_HEIGHT; ++y) {
			if (isInRange(x + uxpos, y + uypos)) {
				size_t brushIdx = ConvertToIndex(x, y, BRUSH_WIDTH);
				size_t boardIdx = ConvertToIndex(x, y, this->width);

				this->board[boardIdx] += this->brush[brushIdx];
				this->board[boardIdx] = this->board[boardIdx] > 1.0f ? 1.0f : this->board[boardIdx];
			}
		}
}

void BrushBoard::erasePaint(double xpos, double ypos)
{
	uint32_t uxpos = static_cast<uint32_t>(xpos);
	uint32_t uypos = static_cast<uint32_t>(ypos);

	for (uint32_t x = 0; x < BRUSH_WIDTH; ++x)
		for (uint32_t y = 0; y < BRUSH_HEIGHT; ++y) {
			if (isInRange(x + uxpos, y + uypos)) {
				size_t brushIdx = ConvertToIndex(x, y, BRUSH_WIDTH);
				size_t boardIdx = ConvertToIndex(x, y, this->width);

				this->board[boardIdx] -= this->brush[brushIdx];
				this->board[boardIdx] = this->board[boardIdx] < 0.0f ? 0.0f : this->board[boardIdx];
			}
		}
}

void BrushBoard::moveViewPoint(double xpos, double ypos)
{
	static glm::vec2 previousPoint(xpos, ypos);

	const float sensitivity = 0.003f;
	glm::vec2 diff = previousPoint - glm::vec2(xpos, ypos);

	this->viewPoint += sensitivity * diff;
}

bool isEqualPair(std::pair<float, float> p1, std::pair<float, float> p2) {
	bool comp1 = std::abs(p1.first  - p2.first ) < std::numeric_limits<float>::epsilon();
	bool comp2 = std::abs(p1.second - p2.second) < std::numeric_limits<float>::epsilon();

	return comp1 && comp2;
}

void BrushBoard::setCutPoint(double xpos, double ypos)
{
	if (!cutPoints.empty() && isEqualPair(cutPoints.front(), std::make_pair(xpos, ypos)))
	{
		uint32_t uxpos = static_cast<uint32_t>(xpos);
		uint32_t uypos = static_cast<uint32_t>(ypos);

		dfsFloodFill(uxpos, uypos);
		
		std::fill(visitPixels.begin(), visitPixels.end(), BoolWrapper(false));
		cutPoints.clear();

		return;
	}

	cutPoints.push_back(std::make_pair(xpos, ypos));

	auto startPoint = cutPoints.size() == 1 ? *(cutPoints.rbegin()) : *(cutPoints.rbegin() + 1);
	auto endPoint = *(cutPoints.rbegin());

	drawLine(startPoint, endPoint);
}


void BrushBoard::drawLine(std::pair<float, float> start, std::pair<float, float> end)
{
	auto delta = (end.second - start.second) / (end.first - start.first);

	//https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm 
	//Bresenham line algorithm

	uint32_t startX = static_cast<uint32_t>(start.first);
	uint32_t endX = static_cast<uint32_t>(end.first);

	//TODO
}

void BrushBoard::dfsFloodFill(uint32_t x, uint32_t y)
{
	auto boardIndex = ConvertToIndex(x, y, width);

	if (!isInRange(x, y) || visitPixels[boardIndex].b)
		return;

	board[boardIndex] = 0.0f;

	dfsFloodFill(x + 1, y);
	dfsFloodFill(x - 1, y);
	dfsFloodFill(x, y - 1);
	dfsFloodFill(x, y + 1);
}