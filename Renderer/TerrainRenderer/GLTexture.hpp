#ifndef GL_TEXTURE_HPP
#define GL_TEXTURE_HPP

#include "EngineAsset.hpp"

typedef struct _TEXTURE
{
	unsigned int activeIndex;
	unsigned int textureID;
} TEXTURE;

class GLTexture : public EngineAsset<GLTexture>
{
private:
	std::vector<TEXTURE> textures;
protected:
public:
	GLTexture();
	GLTexture(const std::vector<std::pair<unsigned int, std::string>>& assetPaths);
	GLTexture(const GLTexture& other);
	GLTexture& operator=(const GLTexture& other) noexcept;
	~GLTexture();
public:
	void loadAsset(const std::vector<std::pair<unsigned int, std::string>>& assetPath);
	void reloadAsset(void);

	void applyTexture(void) const noexcept;
};

#endif