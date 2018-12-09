#include "EngineCubeMap.hpp"
#include "GLShader.hpp"
#include "AssetManager.hpp"
#include "EngineLogger.hpp"
#include <glad/glad.h>
#include "GLResources.hpp"
#include <imgui/imgui.h>
#include "GLDebugger.hpp"

#ifndef _DEBUG
#include "SkyboxShaderCode.hpp"
#endif
#include "obfuscator.hpp"

EngineCubeMap::EngineCubeMap()
	: cubeMap(0)
{
}

EngineCubeMap::EngineCubeMap(const std::string& cubeMapDir, const std::string& extension)
{
	initCubeMap(cubeMapDir, extension);
}

EngineCubeMap::~EngineCubeMap()
{
	if (cubeMap)
		glDeleteTextures(1, &cubeMap);
}

void EngineCubeMap::updateGUI(void)
{
	if (ImGui::TreeNode("CubeMap Setting"))
	{
		ImGui::SliderFloat("Lower Limit", &lowerLimit, 0.0f, 1.0f, "%.1f");
		ImGui::SliderFloat("Uppoer Limit", &upperLimit, 0.0f, 1.0f, "%.1f");
		ImGui::ColorEdit3("Fog Color", &skycolor[0]);

		ImGui::TreePop();
	}
}

void EngineCubeMap::updateScene(float dt)
{
	skyboxShader->useProgram();
	skyboxShader->sendUniform("lowerLimit", lowerLimit);
	skyboxShader->sendUniform("upperLimit", upperLimit);
	skyboxShader->sendUniform("skycolor", skycolor);
	skyboxShader->sendUniform("cubeMap", 0);
}


bool EngineCubeMap::initCubeMap(const std::string& cubeMapDir, const std::string& extension)
{
#ifdef _DEBUG
	assetManager = std::make_unique<AssetManager>();
#endif
	try
	{
#ifdef _DEBUG
		skyboxShader = assetManager->addAsset<GLShader, std::string>({
			OBFUSCATE("../resources/shader/skybox_vs.glsl"),
			OBFUSCATE("../resources/shader/skybox_fs.glsl")
			});
#else
		skyboxShader = std::make_unique<GLShader>();
		skyboxShader->loadAssetRaw(SKYBOX_VS, SKYBOX_FS);
#endif
	}
	catch (std::exception e)
	{
		EngineLogger::getConsole()->error(OBFUSCATE("Intializing Skybox failed. ({})"), e.what());
		return false;
	}

	skyboxShader->useProgram();
	skyboxShader->sendUniform(OBFUSCATE("cubeMap"), 0);
	lowerLimit = 0.8f;
	upperLimit = 0.0f;
	skycolor = glm::vec3(0.5f);

	if (!skyboxMesh.initWithFixedShape(MeshShape::CUBE_TRIANGLES))
		return false;

	return true;
}