#ifndef UTIL_HPP
#define UTIL_HPP

#include <stdint.h>

class Util
{
public:
#ifdef _DEBUG
	static void ApplyDepthOfField(uint32_t framebuffer, int width, int height, float zNear, float zFar);
#endif
};

#endif