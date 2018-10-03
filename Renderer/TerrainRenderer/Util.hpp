#ifndef UTIL_HPP
#define UTIL_HPP

#include <stdint.h>
#include <cmath>

namespace Util
{
#ifdef _DEBUG
	void ApplyDepthOfField(uint32_t framebuffer, int width, int height, float zNear, float zFar);
#endif

	template <typename T>
	inline T clamp(T value, T begin, T end)
	{
		if (value < begin)
			return begin;
		else if (value > end)
			return end;
		else
			return value;
	}
	template <typename T>
	inline T fract(T value)
	{
		return value - static_cast<int>(value);
	}
	template <typename T>
	inline T interpolate(T v1, T v2, float ratio)
	{
		ratio = clamp(ratio, 0.0f, 1.0f);
		return v1 * (1.0f - ratio) + v2 * ratio;
	}
};

#endif