/**
* @file		GLTexture.hpp
* @author	Shinjihong
* @date		31 September 2018
* @version	1.0.0
* @brief	OpenGL Texture Wrapper
* @details	Wrapping OpenGL Texture with texture ID and it's own active index. this will be managed by AssetManager class.
* @see		AssetManager
*/

#ifndef GL_TEXTURE_HPP
#define GL_TEXTURE_HPP

#include "EngineAsset.hpp"

typedef struct _TEXTURE
{
	uint32_t activeIndex;
	uint32_t textureID;
} TEXTURE;

class GLTexture : public EngineAsset
{
private:
	std::vector<TEXTURE> textures;
	
protected:
public:
	GLTexture();
	GLTexture(const std::vector<std::pair<uint32_t, std::string>>& assetPaths);
	GLTexture(const GLTexture& other);
	GLTexture& operator=(const GLTexture& other) noexcept;
	~GLTexture();
public:
	void loadAsset(const std::vector<std::pair<uint32_t, std::string>>& assetPath);
	virtual void reloadAsset(void);

	void applyTexture(void) const noexcept;
};

#endif