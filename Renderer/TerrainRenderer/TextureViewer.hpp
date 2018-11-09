/**
* @file		TextureViewer.hpp
* @author	Shinjihong
* @date		21 September 2018
* @version	1.0.0
* @brief	OpenGL Texture Viewer
* @details	this class is for debugging by viewing textures at run-time. 
* @see		
*/

#ifndef TEXTURE_VIEWER_HPP
#define TEXTURE_VIEWER_HPP

#include <vector>
#include <glm/vec2.hpp>
#include "GLMesh.hpp"

template <typename T>
using uPtr = std::unique_ptr<T>;

class GLShader;

struct TextureView
{
	glm::vec2 position;
	glm::vec2 scale;
	uint32_t textureID;
};

class TextureViewer
{
private:
	GLMesh mesh;
	uPtr <GLShader> viewShader;
	std::vector <TextureView> textureViews;
	std::vector <TextureView> depthTextureViews;
public:
	TextureViewer();
	~TextureViewer();
	TextureViewer(const TextureViewer& other) = delete;
	TextureViewer& operator=(const TextureViewer& other) = delete;
public:
	bool initTextureViewer(void);

	void addTextureView(glm::vec2 centerPos, glm::vec2 scale, uint32_t textureID);
	void addDepthTextureView(glm::vec2 centerPos, glm::vec2 scale, uint32_t textureID);

	void renderViewer(float zNear, float zFar) const;
	
	inline void clearViewer(void) 
	{
		textureViews.clear();
		depthTextureViews.clear();
	}
};

#endif