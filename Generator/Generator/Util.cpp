#include "Util.hpp"


#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
}