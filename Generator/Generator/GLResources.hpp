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
public:
	static uint32_t CreateTexture2D(const std::string& path, bool gamma);
	static uint32_t LoadPresetImage(const std::string& path, int numExtension);
};

#endif