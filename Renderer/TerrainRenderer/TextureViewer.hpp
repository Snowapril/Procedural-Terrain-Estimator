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
	unsigned int textureID;
};

class TextureViewer
{
private:
	std::vector <TextureView> textureViews;
	std::unique_ptr <GLShader> viewShader;
	GLMesh mesh;
public:
	TextureViewer();
	~TextureViewer();
	TextureViewer(const TextureViewer& other) = delete;
	TextureViewer& operator=(const TextureViewer& other) = delete;
public:
	bool initTextureViewer(void);

	void addTextureView(glm::vec2 centerPos, glm::vec2 scale, unsigned int textureID);
	void renderViewer(void) const;
	
	inline void clearViewer(void) 
	{
		textureViews.clear();
	}
};

#endif