#include "NoiseGUI.hpp"
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <imgui/imgui_internal.h>
#include "GLShader.hpp"
#include "Estimator.hpp"
#include "GLResources.hpp"
#include <experimental/filesystem>

using namespace ImGui;

#include <opencv2/imgcodecs.hpp>
using namespace cv;

bool NoiseGUI::isInstanciated = false;

NoiseGUI::NoiseGUI()
	: presetBlend(0.0f), presetImage(0), isGUIOpen(true), isSaveButtonPushed(false)
{
	assert(!isInstanciated);
	isInstanciated = true;
}

NoiseGUI::~NoiseGUI()
{
	isInstanciated = false;
	ImGui_ImplGlfwGL3_Shutdown();
}

bool NoiseGUI::initGUI(GLFWwindow* window)
{
	if (!ImGui_ImplGlfwGL3_Init(window, true))
		return false;

	namespace fs = std::experimental::filesystem;
	for (auto& p : fs::directory_iterator("../resources/texture/preset")) 
	{
		if (p.path().extension() == ".png")
		{
			const std::string filename = p.path().filename().string();
			presetImages.emplace_back(GLResources::LoadPresetImage(p.path().string(), 3), filename);
		}
	}

	return true;
}

void NoiseGUI::updateGUI(glm::vec2 rect)
{
	ImGui_ImplGlfwGL3_NewFrame();

	ImGui::Begin("Generator & Estimator", &isGUIOpen, ImVec2(0, 0), 0.5f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize(ImVec2(rect.x, rect.y));
}

void NoiseGUI::endUpdate(uint32_t frameTexture)
{
	if (ImGui::TreeNode("Preset Image"))
	{
		ImGui::SliderFloat("Blend", &presetBlend, 0.0f, 1.0f);
		for (const auto& preset : presetImages)
		{
			ImGui::Image((void*)preset.texture, ImVec2(80.0f, 80.0f));
			ImGui::SameLine(150);
			const char* const label = preset.name.c_str();
			if (ImGui::Button(label))
				presetImage = preset.texture;
		}
		ImGui::TreePop();
	}

	if (ImGui::CollapsingHeader("Key Support", 0, true, true))
	{
		ImGui::Text("Number 1 : Voronoi");
		ImGui::Text("Number 2 : fbM");
		ImGui::Text("Number 3 : Simplex");
		ImGui::Text("R        : Reset");
		ImGui::Text("B        : Brush (+)");
		ImGui::Text("N        : Brush (-)");
		ImGui::Text("Z        : Zoom in");
		ImGui::Text("X        : Zoom out");
		ImGui::Text("C        : Polygonal Lasso");
		ImGui::Text("F        : Free Form Lasso");
		ImGui::Text("M        : Move Scroll");
	}
	
	if (ImGui::CollapsingHeader("Application Info", 0, true, true))
	{
		char* glInfos = (char*)glGetString(GL_VERSION);
		char* hardwareInfos = (char*)glGetString(GL_RENDERER);
		
		ImGui::Text("OpenGL Version :");
		ImGui::Text(glInfos);
		ImGui::Text("Hardware Informations :");
		ImGui::Text(hardwareInfos);
		ImGui::Text("\nFramerate %.2f FPS / Frametime %.4f ms", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
	}

	if (ImGui::CollapsingHeader("About", 0, true, true))
	{
		ImGui::Text("Procedrual Terrain Estimator");
		ImGui::Text("Map Generator by snowapril");
		ImGui::Text("Email: sinjihng@pusan.ac.kr");
	}

	if (ImGui::Button("Save as image"))
	{
		saveCurrentTexture("../resources/texture/terrain/height16bit2.png", 2048, 2048, frameTexture);
	}

	if (ImGui::Button("BlendMap Coloring")) 
	{
		auto& estimator = Estimator::getMutableInstance();

		estimator.initHMapData(frameTexture, 2048, 2048);
		estimator.blendmapColoring();
		estimator.generateBlendMap("../resources/texture/terrain/splatMap.png", 2048, 2048);
	}

	ImGui::End();
}

void NoiseGUI::renderGUI(void) const
{
	ImGui::Render();
}


void NoiseGUI::saveCurrentTexture(const std::string& path, int width, int height, uint32_t texture)
{
	std::vector<unsigned short> data(width * height);
	
	glBindTexture(GL_TEXTURE_2D, texture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_SHORT, (void*)&data[0]);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	Mat src = Mat(cv::Size(width, height), CV_16UC1, (void*)&data[0]);
	imwrite(path, src);
}