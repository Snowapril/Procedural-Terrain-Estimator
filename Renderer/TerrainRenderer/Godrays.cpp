#include "Godrays.hpp"
#include <glad/glad.h>
#include "EngineLogger.hpp"
#include "AssetManager.hpp"
#include "LightSourceWrapper.hpp"
#include "EngineCamera.hpp"
#include "GLResources.hpp"
#include "EngineLogger.hpp"
#include "GLShader.hpp"
#include <glm/gtc/matrix_transform.hpp>

Godrays::Godrays()
{

}

Godrays::~Godrays()
{

}


bool Godrays::initGodrays(int width, int height)
{
    screenBuffer = std::make_unique<GLFramebuffer>();

    screenBuffer->initFramebuffer();
	screenBuffer->attachColorTexture(width, height, GL_REPEAT, false);

    if (!screenBuffer->configureFramebuffer())
        return false;

    return true;
}

/*



#ifdef _DEBUG
bool Godrays::initLensFlare(float spacing, size_t numTextures)
{
	const std::string basePath = "../resources/texture/lensFlare/tex";
	const std::string extension = ".png";

	this->spacing = spacing;

	for (size_t i = 1; i <= numTextures; ++i)
	{
		uint32_t texture = GLResources::CreateTexture2D(basePath + std::to_string(i) + extension, false);

		if (texture == 0)
		{
			EngineLogger::getConsole()->error("Lens Flare Initialization Failed");
			return false;
		}

		flareTextures.push_back(texture);
	}

	if (!assetManager)
		assetManager = std::make_unique<AssetManager>();

	try
	{
		flareShader = assetManager->addAsset<GLShader, std::string>({
			"../resources/shader/flare_vs.glsl",
			"../resources/shader/flare_fs.glsl",
			});
	}
	catch (std::exception e)
	{
		EngineLogger::getConsole()->error("Lens Flare Initialization Failed");
		return false;
	}

	flareShader->useProgram();
	flareShader->sendUniform("effectTexture", 0);

	flareMesh.initWithFixedShape(MeshShape::QUAD_TRIANGLE_STRIP, 0.3f);

	return true;
}
#endif
#ifdef _DEBUG
void Godrays::drawLensFlare(const EngineCamera& camera, const LightSourceWrapper& lightSource, const glm::vec2& centerScreen) const
{
	flareShader->useProgram();

	const auto& dirLights = lightSource.getDirLights();
	glm::vec4 position = glm::vec4(-dirLights[0].direction, 1.0);

	position = (camera.getProjectMatrix() * camera.getViewMatrix()) * position;

	if (position.w <= 0.0f)
		return;

	float x = (position.x / position.w + 1.0f) * 0.5f;
	float y = 1 - ((position.y / position.w + 1.0f) * 0.5f);

	glm::vec2 sunCoords(x, y);

	glm::vec2 sunToCenter = centerScreen - sunCoords;
	float brightness = 1.0f - (glm::length(sunToCenter) * 1.66667f);

	if (brightness > 0.0f)
	{
		flareShader->sendUniform("brightness", brightness);
		glActiveTexture(GL_TEXTURE0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		for (size_t i = 0; i < flareTextures.size(); ++i)
		{
			glm::vec3 flarePosition(sunToCenter * static_cast<float>(i), 0.0f);

			glm::mat4 model(1.0f);
			model = glm::translate(model, flarePosition);

			flareShader->sendUniform("model", model);

			glBindTexture(GL_TEXTURE_2D, flareTextures[i]);
			flareMesh.drawMesh(GL_TRIANGLE_STRIP);
		}
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
	}
}
#endif

*/