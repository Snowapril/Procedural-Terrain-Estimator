#ifndef TEXTURE_VIEWER_HPP
#define TEXTURE_VIEWER_HPP

#include <vector>
#include <glm/vec2.hpp>
#include "GLMesh.hpp"

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
	std::vector <TextureView> textureViews;
	std::vector <TextureView> depthTextureViews;

	std::unique_ptr <GLShader> viewShader;
	GLMesh mesh;
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