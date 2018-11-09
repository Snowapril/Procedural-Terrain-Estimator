#include "Util.hpp"

namespace Util
{
	Rect::Rect()
	{
	}

	Rect::Rect(glm::vec2 lt, glm::vec2 rb)
		: leftTop(lt), rightBottom(rb)
	{

	}

	Rect::Rect(float left, float top, float right, float bottom)
		: leftTop(left, top), rightBottom(right, bottom)
	{
	}

	Rect::Rect(const Rect & other)
	{
		this->leftTop	  = other.leftTop;
		this->rightBottom = other.rightBottom;
	}

	Rect & Rect::operator=(const Rect & other)
	{
		if (&other == this)
			return *this;

		this->leftTop	  = other.leftTop;
		this->rightBottom = other.rightBottom;
		
		return *this;
	}

	Rect::Rect(Rect && other)
	{
		this->leftTop	  = std::move(other.leftTop);
		this->rightBottom = std::move(other.rightBottom);
	}

	Rect & Rect::operator=(Rect && other)
	{
		if (&other == this)
			return *this;

		this->leftTop	  = std::move(other.leftTop);
		this->rightBottom = std::move(other.rightBottom);

		return *this;
	}

	glm::vec2 Rect::getScale(void) const
	{
		return rightBottom - leftTop;
	}

	glm::vec2 Rect::getLeftTop(void) const
	{
		return leftTop;
	}

	glm::vec2 Rect::getRightBottom(void) const
	{
		return rightBottom;
	}

	void Rect::setLeftTop(glm::vec2 newValue)
	{
		leftTop = newValue;
	}
	void Rect::setRightBottom(glm::vec2 newValue)
	{
		rightBottom = newValue;
	}
}