#include "GLTexture.hpp"
#include <glad/glad.h>
#include "GLResources.hpp"
#include <filesystem>
#include "obfuscator.hpp"

GLTexture::GLTexture()
{
}

GLTexture::GLTexture(const std::vector<std::pair<uint32_t, std::string>>& assetPaths)
{
	loadAsset(assetPaths);
}

GLTexture::GLTexture(const GLTexture & other)
	: textures(other.textures)
{
}

GLTexture & GLTexture::operator=(const GLTexture & other) noexcept
{
	if (&other != this)
		return *this;

	textures = other.textures;

	return *this;
}

GLTexture::~GLTexture()
{
	for (auto& texture : textures)
		glDeleteTextures(1, &texture.textureID);
}

/**
* @ brief		load asset(textures) with texture active index and texture paths.
* @ details		load asset from given paths, if paths not exist or not image form then CreateTexture2D return 0.
				if get 0 from CreateTexture2D, throw std::exception. 
*/
void GLTexture::loadAsset(const std::vector<std::pair<uint32_t, std::string>>& assetPath)
{
	namespace fs = std::experimental::filesystem;

	int iter = 0, width, height;
	
	std::vector<TEXTURE> loadAssets = textures;

	if (assetPath.size() == 0)
	{
		for (const auto& pair : assetPaths)
		{
			const uint32_t texture = GLResources::CreateTexture2D(pair.second, width, height, true);
			if (texture == 0)
			{
				return;
			}

			const int64_t time = fs::last_write_time(pair.second).time_since_epoch().count();
			assetPaths[iter].first = time;
			loadAssets[iter].textureID = texture;
			loadAssets[iter].textureSize = glm::ivec2(width, height);
			++iter;
		}
	}
	else
	{
		assetPaths.resize(assetPath.size());

		for (const auto& pair : assetPath)
		{
			const uint32_t texture = GLResources::CreateTexture2D(pair.second, width, height, true);
			const int64_t time = fs::last_write_time(pair.second).time_since_epoch().count();

			if (texture == 0) 
			{
				std::string errorMsg = OBFUSCATE("Cannot load texture from ") + pair.second;
				const char* msgSrc = errorMsg.c_str();
				MessageBox(NULL, msgSrc, OBFUSCATE("Resource Load Error"), MB_OK);
			}

			assetPaths[iter].first = time;
			assetPaths[iter].second = pair.second;

			loadAssets.push_back({ glm::ivec2(width, height), pair.first, texture});

			++iter;
		}
	}

	for (auto& texture : textures)
		glDeleteTextures(1, &texture.textureID);

	textures.swap(loadAssets);
}

/**
* @ brief		reload asset
* @ details		delete current textures and replace it with new textures.
*/
void GLTexture::reloadAsset(void)
{
	loadAsset({});
}

/**
* @ brief		before draw call, if you want to apply texture, call this first.
* @ details		bind texture with specific index.
*/
void GLTexture::applyTexture(void) const noexcept
{
	for (const auto& texture : textures)
	{
		glActiveTexture(GL_TEXTURE0 + texture.activeIndex);
		glBindTexture(GL_TEXTURE_2D, texture.textureID);
	}
}

uint32_t GLTexture::getTextrueID(uint32_t idx) const
{
	return textures[idx].textureID;
}

void GLTexture::setTextureID(uint32_t idx, uint32_t textureID)
{
	textures[idx].textureID = textureID;
}

glm::ivec2 GLTexture::getTextureSize(uint32_t idx) const
{
	return textures[idx].textureSize;
}