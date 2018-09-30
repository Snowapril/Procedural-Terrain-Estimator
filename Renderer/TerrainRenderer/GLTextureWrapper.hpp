#ifndef GL_TEXTURE_WRAPPER_HPP
#define GL_TEXTURE_WRAPPER_HPP

#include <stdint.h>
#include <vector>

class GLShader;

class GLTextureWrapper
{
private:
	using AtlasVec = std::vector<std::pair<uint32_t, uint32_t>>;

	uint32_t stride;
	uint32_t texWrapID;

	uint32_t width;
	uint32_t height;

	AtlasVec configures;
protected:
public:
	GLTextureWrapper();
	~GLTextureWrapper();
public:
	bool initStorage(uint32_t width, uint32_t height, uint32_t stride);
	bool fillTexAttribPointer(uint32_t offset,  std::string&& dir, std::vector<std::string>&& names);
	void sendTextureAtlas(const GLShader& shader, uint32_t bindPosition) const;
};

#endif