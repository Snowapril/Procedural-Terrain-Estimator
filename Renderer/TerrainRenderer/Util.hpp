#ifndef UTIL_HPP
#define UTIL_HPP

#include <stdint.h>

class Util
{
public:
	static void ApplyDepthOfField(uint32_t framebuffer, int width, int height, float zNear, float zFar);
};

#endif