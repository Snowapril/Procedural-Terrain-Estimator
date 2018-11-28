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
	static uint32_t CreateTexture2D(const std::string& path, bool gamma);
#ifdef _DEBUG
	static uint32_t CreateTexture2DApplying3x3AverageFilter(const std::string& path, std::size_t& retWidth, std::size_t& retHeight, bool gamma);
#endif
	static uint32_t CreateTexture2D(const std::string& path, int& retWidth, int& retHeight, bool gamma);
	static uint32_t CreateSkybox(const std::string& skyboxDir, const std::string& extension);
	static uint32_t CreateHDREnvMap(const std::string& path);
};

#endif