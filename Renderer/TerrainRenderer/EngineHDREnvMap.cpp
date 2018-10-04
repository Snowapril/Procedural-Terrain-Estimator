#include "EngineHDREnvMap.hpp"
#include "GLShader.hpp"
#include "AssetManager.hpp"
#include "EngineLogger.hpp"
#include <glad/glad.h>
#include "GLResources.hpp"
#include "EngineProperty.hpp"
#include "GLDebugger.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "EngineCamera.hpp"

constexpr int HDR_RESOLUTION = CLIENT_WIDTH;

EngineHDREnvMap::EngineHDREnvMap()
{
}

EngineHDREnvMap::EngineHDREnvMap(const std::string& cubeMapDir, const std::string& extension)
{
	initCubeMap(cubeMapDir, extension);
}

EngineHDREnvMap::~EngineHDREnvMap()
{
}

bool EngineHDREnvMap::initCubeMap(const std::string& cubeMapDir, const std::string& extension)
{
	EngineCubeMap::initCubeMap(cubeMapDir, extension);

	uint32_t hdrMap;

	if ((hdrMap = GLResources::CreateHDREnvMap(cubeMapDir + extension)) == 0)
		return false;

	GLShader bakeShader;
	try
	{
		bakeShader.loadAsset({
			"../resources/shader/equiRectangularMapToCubemap_vs.glsl",
			"../resources/shader/equiRectangularMapToCubemap_fs.glsl",
		});
	}
	catch (std::exception e)
	{
		EngineLogger::getConsole()->error("Failed to load shader (failed to baking hdr environment map to cube map)");
		return false;
	}

	GLMesh cubeMesh;
	
	if (!cubeMesh.initWithFixedShape(MeshShape::CUBE_TRIANGLES))
	{
		EngineLogger::getConsole()->error("Failed to init quad mesh");
		return false;
	}

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f, 0.0f), glm::vec3(0.0f, 0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
	};

	uint32_t captureFBO, captureRBO;

	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, HDR_RESOLUTION, HDR_RESOLUTION);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	glGenTextures(1, &cubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	for (uint32_t i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, HDR_RESOLUTION, HDR_RESOLUTION, 0, GL_RGB, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	bakeShader.useProgram();
	bakeShader.sendUniform("equiRectangularMap", 0);
	bakeShader.sendUniform("projection", captureProjection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrMap);

	glViewport(0, 0, HDR_RESOLUTION, HDR_RESOLUTION);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

	for (uint32_t i = 0; i < 6; ++i)
	{
		bakeShader.sendUniform("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cubeMesh.drawMesh(GL_TRIANGLES);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void EngineHDREnvMap::drawScene(const EngineCamera& camera) const
{
	skyboxShader->useProgram();

	glDepthFunc(GL_LEQUAL);
	
	camera.sendVP(*skyboxShader, true);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	skyboxMesh.drawMesh(GL_TRIANGLES);
	glDepthFunc(GL_LESS);
}