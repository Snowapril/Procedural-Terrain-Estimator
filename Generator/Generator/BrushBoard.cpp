#include "BrushBoard.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

constexpr uint32_t  BRUSH_WIDTH		  = 110;
constexpr uint32_t  BRUSH_HEIGHT	  = 110;
constexpr uint32_t  HALF_BRUSH_WIDTH  = BRUSH_WIDTH / 2;
constexpr uint32_t  HALF_BRUSH_HEIGHT = BRUSH_HEIGHT / 2;
constexpr uint32_t	COUNTER_HUDDLE	  = 10;
constexpr float		BRUSH_DECAL		  = 0.002f;

BrushBoard::BrushBoard()
	: isScreenClicked(false), counter(0U), width(0U), height(0U), brushTexture(0U), paintFuncPtr(nullptr), paintMode(BrushMode::NONE)
{
}

BrushBoard::BrushBoard(uint32_t width, uint32_t height) 
	: isScreenClicked(false), counter(0U), width(0U), height(0U), brushTexture(0U), paintFuncPtr(nullptr), paintMode(BrushMode::NONE)
{
	if (!initBrushBoard(width, height))
		throw std::exception("Failed to initialize brush board");
}

BrushBoard::~BrushBoard()
{
	if (brushTexture)
		glDeleteTextures(GL_TEXTURE_2D, &brushTexture);
}

bool BrushBoard::initBrushBoard(uint32_t width, uint32_t height)
{
	this->width = width;
	this->height = height;

	brush.resize(BRUSH_WIDTH * BRUSH_HEIGHT);
	board.resize(this->width * this->height, 0.0f);
	visitPixels.resize(this->width * this->height, { false });

	glGenTextures(1, &brushTexture);
	glBindTexture(GL_TEXTURE_2D, brushTexture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, this->width, this->height, 0, GL_RED, GL_FLOAT, nullptr);
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

	paintFuncPtr(xpos, ypos);
}

void BrushBoard::processWheelOffset(double yoffset)
{
	
}

void BrushBoard::processMouseBtn(int button, int action)
{
	using namespace std::placeholders;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) 
	{
		paintMode = BrushMode::PLUS;
		paintFuncPtr = std::bind(&BrushBoard::writePaint, this, _1, _2);
		isScreenClicked = true;
	}
	else
	{
		paintMode = BrushMode::NONE;	
		paintFuncPtr = nullptr;
		isScreenClicked = false;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) 
	{
		paintMode = BrushMode::MINUS;
		paintFuncPtr = std::bind(&BrushBoard::erasePaint, this, _1, _2);
		isScreenClicked = true;
	}
	else 
	{
		paintMode = BrushMode::NONE;
		paintFuncPtr = nullptr;
		isScreenClicked = false;
	}
}

void BrushBoard::processToggleKey(int key, int scancode, int action)
{
}

void BrushBoard::setPaintMode(BrushMode mode)
{
	this->paintMode = mode;

	switch (this->paintMode) 
	{
	case BrushMode::PLUS :
		break;
	case BrushMode::MINUS : 
		break;
	case BrushMode::CUT :
		break;
	case BrushMode::CLEAR:
		std::fill(board.begin(), board.end(), 0.0f);
		break;
	case BrushMode::NONE:
		break;
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
}

void BrushBoard::erasePaint(double xpos, double ypos)
{
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

	}

	cutPoints.push_back(std::make_pair(xpos, ypos));

	auto startPoint = cutPoints.size() == 1 ? *(cutPoints.rbegin()) : *(cutPoints.rbegin() + 1);
	auto endPoint = *(cutPoints.rbegin());
}