#include "GLTextureWrapper.hpp"
#include <glad/glad.h>

#ifdef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include <stb/stb_image.h>

#include "GLShader.hpp"

GLTextureWrapper::GLTextureWrapper()
{
}

GLTextureWrapper::~GLTextureWrapper()
{
}

bool GLTextureWrapper::initStorage(uint32_t width, uint32_t height, uint32_t stride)
{
	glGenTextures(1, &texWrapID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texWrapID);

	this->width = width;
	this->height = height;
	this->stride = stride;

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, this->width, this->height, this->stride);

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	return true;
}

bool GLTextureWrapper::fillTexAttribPointer(uint32_t offset,  std::string&& dir, std::vector<std::string>&& names)
{
	const std::string& directory = std::move(dir);
	const std::vector<std::string>& fileNames = std::move(names);

	if (offset + fileNames.size() >= stride)
		return false;

	configures.push_back(std::make_pair(offset, fileNames.size()));

	glBindTexture(GL_TEXTURE_2D_ARRAY, texWrapID);

	stbi_set_flip_vertically_on_load(true);
	for (uint32_t i = 0; i < fileNames.size(); ++i)
	{
		int tW, tH, tC;
		unsigned char* data = stbi_load((directory + fileNames[i]).c_str(), &tW, &tH, &tC, 0);

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, offset + i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
	stbi_set_flip_vertically_on_load(false);

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	return true;
}

void GLTextureWrapper::sendTextureAtlas(const GLShader& shader, uint32_t bindPosition) const
{
	glActiveTexture(GL_TEXTURE0 + bindPosition);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texWrapID);

	auto size = configures.size();
	for (uint32_t i = 0; i < size; ++i)
	{
		shader.sendUniform("AtlasAttrib[" + std::to_string(i) + "]", static_cast<int>(configures[i].first));
		shader.sendUniform("AtlasAttrib[" + std::to_string(i) + "]", static_cast<int>(configures[i].second));
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}