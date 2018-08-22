#ifndef GL_RESOURCES_HPP
#define GL_RESOURCES_HPP

#include <string>

class GLResources
{
public:
	static unsigned int CreateTexture2D(const std::string& path, bool gamma);
};

#endif