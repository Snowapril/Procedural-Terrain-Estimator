/**
* @file		EngineProperty.hpp
* @author	Shinjihong
* @date		27 August 2018
* @version	1.0.0
* @brief	provide method for Creating and Loading resources (textures, sound, ..)
* @see
*/

#ifndef GL_RESOURCES_HPP
#define GL_RESOURCES_HPP

#include <string>

class GLResources
{
private:

public:
	static unsigned int CreateTexture2D(const std::string& path, bool gamma);
	static unsigned int CreateTexture2DApplying3x3AverageFilter(const std::string& path, std::size_t& retWidth, std::size_t& retHeight, bool gamma);
	static unsigned int CreateTexture2D(const std::string& path, std::size_t& retWidth, std::size_t& retHeight, bool gamma);
	static void CreateSimplexNoise2D(int width, int height, unsigned int seed);
};

#endif