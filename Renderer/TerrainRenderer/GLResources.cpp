#include "GLResources.hpp"
#include <glad/glad.h>
#include "EngineLogger.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include "obfuscator.hpp"

#include <opencv2/imgcodecs.hpp>
using namespace cv;

uint32_t GLResources::CreateTexture2D(const std::string& path, bool gamma)
{
	stbi_set_flip_vertically_on_load(true);

	int width, height, nChannels;
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nChannels, 0);
	if (data == nullptr || width == 0 || height == 0 || nChannels == 0)
	{
		EngineLogger::getConsole()->error(OBFUSCATE("Cannot load texture from {}"), path);

		return 0;
	}

	uint32_t texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	GLenum format, internalFormat;
	switch (nChannels)
	{
	case 1:
		format = GL_RED;
		internalFormat = GL_RED;
		break;
	case 3:
		format = GL_RGB;
		internalFormat = gamma ? GL_SRGB : GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
		break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGB ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGB ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);
	stbi_set_flip_vertically_on_load(false);

	const auto dirIdx = path.find_last_of(OBFUSCATE("/"));
	const std::string filename = path.substr(dirIdx + 1);

	EngineLogger::getConsole()->info(OBFUSCATE("Load Texture {} finished"), filename);

	return texture;
}

uint32_t GLResources::CreateTexture2D(const std::string& path, int& retWidth, int& retHeight, bool gamma)
{
	auto resource = imread(path, IMREAD_ANYCOLOR | IMREAD_ANYDEPTH | IMREAD_UNCHANGED);

	retWidth  = resource.rows;
	retHeight = resource.cols;

	uint32_t texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	GLenum format, internalFormat;
	switch (resource.channels())
	{
	case 1:
		format = GL_RED;
		internalFormat = GL_RED;
		break;
	case 3:
		format = GL_RGB;
		internalFormat = gamma ? GL_SRGB : GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
		break;
	}

	if (resource.depth() == 2) 
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, resource.rows, resource.cols, 0, format, GL_UNSIGNED_SHORT, (unsigned short*)resource.data);
	else 
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, resource.rows, resource.cols, 0, format, GL_UNSIGNED_BYTE, resource.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGB ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGB ? GL_REPEAT : GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	const auto dirIdx = path.find_last_of(OBFUSCATE("/"));
	const std::string filename = path.substr(dirIdx + 1);

	EngineLogger::getConsole()->info(OBFUSCATE("Load Texture {} finished"), filename);

	return texture;
}

#ifdef _DEBUG
uint32_t GLResources::CreateTexture2DApplying3x3AverageFilter(const std::string& path, std::size_t& retWidth, std::size_t& retHeight, bool gamma)
{
	stbi_set_flip_vertically_on_load(true);

	int width, height, nChannels;
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nChannels, 0);
	if (data == nullptr || width == 0 || height == 0 || nChannels == 0)
	{
		EngineLogger::getConsole()->error(OBFUSCATE("Cannot load texture from {}"), path);
		return 0;
	}

	std::vector<unsigned short> avgData;
	avgData.reserve(width * height);

	auto getIndex = [&width, &height](int i, int j) -> int {
		return i + j * width;
	};

	auto isInBound = [&width, &height, &data](int i, int j) -> bool {
		
		return i >= 0 && i < width && j >= 0 && j < height;
	};

	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
		{
			int count = 0;
			int sum = 0;

			for (int r = -1; r <= 1; ++r)
				for (int c = -1; c <= 1; ++c)
				{
					if (isInBound(i + r, j + c))
					{
						++count;
						sum += data[getIndex(i + r, j + c)];
					}
				}

			sum = sum / count;
			avgData.push_back(sum);
		}
	
	retWidth  = width;
	retHeight = height;

	uint32_t texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	GLenum format, internalFormat;
	switch (nChannels)
	{
	case 1:
		format = GL_RED;
		internalFormat = GL_RED;
		break;
	case 3:
		format = GL_RGB;
		internalFormat = gamma ? GL_SRGB : GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
		break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGB ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGB ? GL_REPEAT : GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);
	stbi_set_flip_vertically_on_load(false);

	const auto dirIdx = path.find_last_of(OBFUSCATE("/"));
	const std::string filename = path.substr(dirIdx + 1);

	EngineLogger::getConsole()->info(OBFUSCATE("Load Texture {} finished"), filename);

	return texture;
}
#endif

uint32_t GLResources::CreateSkybox(const std::string& skyboxDir, const std::string& extension)
{	
	std::string paths[] = {
		OBFUSCATE("right."),
		OBFUSCATE("left."),
		OBFUSCATE("top."),
		OBFUSCATE("bottom."),
		OBFUSCATE("back."),
		OBFUSCATE("front."),
	};

	uint32_t texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	int width, height, nChannels;

	for (int i = 0; i < 6; ++i)
	{
		std::string fullpath = skyboxDir + paths[i] + extension;
		unsigned char* data = stbi_load(fullpath.c_str(), &width, &height, &nChannels, 0);

		if (data == nullptr || width == 0 || height == 0 || nChannels == 0)
		{
			EngineLogger::getConsole()->error(OBFUSCATE("Cannot load texture from {}"), fullpath);
			stbi_image_free(data);

			return 0;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	EngineLogger::getConsole()->info(OBFUSCATE("Load Cubemap {} finished"), skyboxDir);
	
	return texture;
}

uint32_t GLResources::CreateHDREnvMap(const std::string& path)
{
	stbi_set_flip_vertically_on_load(true);

	uint32_t texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	int width, height, nChannels;

	float* data = stbi_loadf(path.c_str(), &width, &height, &nChannels, 0);

	if (data == nullptr || width == 0 || height == 0 || nChannels == 0)
	{
		EngineLogger::getConsole()->error(OBFUSCATE("Cannot load texture from {}"), path);
		stbi_image_free(data);

		return 0;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	stbi_image_free(data);

	EngineLogger::getConsole()->info(OBFUSCATE("Load Cubemap {} finished"), path);
	stbi_set_flip_vertically_on_load(false);

	return texture;
}