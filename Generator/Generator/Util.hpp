#ifndef UTIL_HPP
#define UTIL_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <utility>	

namespace Util
{
	inline glm::vec2 fract2(const glm::vec2& x)
	{
		return glm::vec2(x[0] - static_cast<int>(x[0]), x[1] - static_cast<int>(x[1]));
	}

	inline float vnoise(float x, float y)
	{
		glm::vec2 n(x, y);

		float F1 = 8.0f;
		for (int i = -1; i <= 1; ++i)
			for (int j = -1; j <= 1; ++j)
			{
				glm::vec2 g(i, j);
				glm::vec2 p(n + g);
				glm::vec2 o(fract2(glm::sin(glm::vec2(glm::dot(p, glm::vec2(127.1f, 311.7f)), glm::dot(p, glm::vec2(269.5f, 183.3f)))) * 43758.5453f));
				o = glm::vec2(0.5f, 0.5f) + 0.41f * glm::sin(glm::vec2(6.2831f, 6.2831f) * o);
				glm::vec2 r(g + o);

				float d = glm::dot(r, r);

				if (d < F1)
					F1 = d;
			}

		return F1;
	}

	inline bool isEqualPair(std::pair<float, float> p1, std::pair<float, float> p2)
	{
		const bool comp1 = std::abs(p1.first - p2.first) < std::numeric_limits<float>::epsilon();
		const bool comp2 = std::abs(p1.second - p2.second) < std::numeric_limits<float>::epsilon();

		return comp1 && comp2;
	}

	inline int sign(float x)
	{
		return x >= 0.0f ? 1 : -1;
	}

	inline bool isInRange(int y, int start_y, int end_y)
	{
		return end_y > start_y ? y <= end_y : y > start_y;
	}

	class Rect
	{
	private:
		glm::vec2 leftTop;
		glm::vec2 rightBottom;

	public:
		Rect();
		Rect(glm::vec2 lt, glm::vec2 rb);
		Rect(float left, float top, float right, float bottom);
		Rect(const Rect& other);
		Rect& operator=(const Rect& other);
		Rect(Rect&& other);
		Rect& operator=(Rect&& other);

	public:
		glm::vec2 getScale(void) const;
		glm::vec2 getLeftTop(void) const;
		glm::vec2 getRightBottom(void) const;
		void setLeftTop(glm::vec2 newValue);
		void setRightBottom(glm::vec2 newValue);

		template <typename T>
		inline bool isInBound(T xpos, T ypos) const 
		{
			return leftTop.x <= xpos && rightBottom.x >= xpos &&
				leftTop.y <= ypos && rightBottom.y >= ypos;
		}
	};
}


#endif