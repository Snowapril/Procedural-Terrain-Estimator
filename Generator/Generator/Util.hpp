#ifndef UTIL_HPP
#define UTIL_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Util
{
	inline glm::vec2 fract2(const glm::vec2& x);
	inline float vnoise(float x, float y);
}

#endif