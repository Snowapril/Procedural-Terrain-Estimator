#include "TextureViewer.hpp"
#include "EngineLogger.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include "EngineAsset.hpp"
#include "GLShader.hpp"

TextureViewer::TextureViewer()
{
}

TextureViewer::~TextureViewer()
{
}

bool TextureViewer::initTextureViewer(void)
{
	try
	{
		viewShader = make_unique_from_list<GLShader, std::string>({
			"../resources/shader/viewer_vs.glsl",
			"../resources/shader/viewer_fs.glsl",
		});
	}
	catch(std::exception e)
	{
		EngineLogger::getConsole()->error("Failed to initialize view shader");
		return false;
	}

	viewShader->useProgram();
	viewShader->sendUniform("textureView", 0);

	if (!mesh.initWithFixedShape(MeshShape::QUAD_TRIANGLE_STRIP))
		return false;

	return true;
}

void TextureViewer::addTextureView(glm::vec2 centerPos, glm::vec2 scale, uint32_t textureID)
{
	textureViews.push_back({ std::move(centerPos), std::move(scale), textureID });
}

void TextureViewer::addDepthTextureView(glm::vec2 centerPos, glm::vec2 scale, uint32_t textureID)
{
	depthTextureViews.push_back({ std::move(centerPos), std::move(scale), textureID });
}

void TextureViewer::renderViewer(float zNear, float zFar) const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	viewShader->useProgram();

	glActiveTexture(GL_TEXTURE0);
	
	viewShader->sendUniform("depthRender", false);
	for (const TextureView& view : textureViews)
	{
		glm::mat4 model(1.0f);

		model = glm::translate(model, glm::vec3(view.position.x, view.position.y, 0.0f));
		model = glm::scale(model, glm::vec3(view.scale.x, view.scale.y, 1.0f));

		viewShader->sendUniform("model", model);
		glBindTexture(GL_TEXTURE_2D, view.textureID);
		mesh.drawMesh(GL_TRIANGLE_STRIP);
	}

	viewShader->sendUniform("depthRender", true);
	for (const TextureView& view : depthTextureViews)
	{
		glm::mat4 model(1.0f);

		model = glm::translate(model, glm::vec3(view.position.x, view.position.y, 0.0f));
		model = glm::scale(model, glm::vec3(view.scale.x, view.scale.y, 1.0f));

		viewShader->sendUniform("model", model);
		glBindTexture(GL_TEXTURE_2D, view.textureID);
		mesh.drawMesh(GL_TRIANGLE_STRIP);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}