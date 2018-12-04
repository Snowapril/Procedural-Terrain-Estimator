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

	StyleColorsDark(&GetStyle());

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
		ImGui::PushItemWidth(210);
		ImGui::SliderFloat("Blend", &presetBlend, 0.0f, 1.0f);
		for (const auto& preset : presetImages)
		{
			ImGui::Image((void*)preset.texture, ImVec2(80.0f, 80.0f));
			ImGui::SameLine(150);
			const char* const label = preset.name.c_str();
			if (ImGui::Button(label))
				presetImage = preset.texture;
		}
		ImGui::PopItemWidth();
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
		ImGui::Text("Blend map & Post-processor by kkorona");
		ImGui::Text("Email: high1uck@pusan.ac.kr");
	}

	if (ImGui::CollapsingHeader("Export Resources"))
	{
		ImGui::TextWrapped("Export height-map and blend-map as images to path resources/texture/terrain.");
		if (ImGui::Button("Export Images..")) 
		{
			auto& estimator = Estimator::getMutableInstance();
			estimator.initHMapData(frameTexture, 2048, 2048);
			ImGui::OpenPopup("Export Images");
		}
		if (ImGui::BeginPopupModal("Export Images", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			auto& estimator = Estimator::getMutableInstance();
			const char* sizeItems[] = { "512", "1024", "2048", "4096" };
			static unsigned int blendMapTexture = 0;

			ImGui::Text("Height-map");
			ImGui::SameLine(230);
			ImGui::Text("Blend-map");

			ImGui::Image((void*)frameTexture, ImVec2(200.0f, 200.0f));
			ImGui::SameLine(230);
			ImGui::Image((void*)blendMapTexture, ImVec2(200.0f, 200.0f));

			static int smoothness_iter = 0;
			static bool normalize_enable = false;
			static int min_height = 0;
			static int max_height = 410;

			ImGui::PushItemWidth(125);
			ImGui::Text("normalize");
			ImGui::Checkbox("enable", &normalize_enable);
			if (normalize_enable) {
				ImGui::SameLine();
				ImGui::InputInt("min_height", &min_height);
				ImGui::SameLine();
				ImGui::InputInt("max_height", &max_height);
			}
			ImGui::Text("smoothness");
			ImGui::SliderInt("smooth iteration", &smoothness_iter, 0, 50);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(200);
			ImGui::Text("Size of resource:");
			static int widthCurrentIndex = 2, heightCurrentIndex = 2;
			ImGui::ListBox("Width", &widthCurrentIndex, sizeItems, IM_ARRAYSIZE(sizeItems));
			ImGui::SameLine();
			ImGui::ListBox("Height", &heightCurrentIndex, sizeItems, IM_ARRAYSIZE(sizeItems));
			ImGui::PopItemWidth();
			//ImGui::ProgressBar()
			//ImGui::BeginTooltip
			if (ImGui::Button("Save Resources"))
			{
				int width = stoi(sizeItems[widthCurrentIndex]), height = stoi(sizeItems[heightCurrentIndex]);
				if (normalize_enable) {
					estimator.normalize(min_height, max_height);
				}
				estimator.blendmapColoring();
				blendMapTexture = estimator.getBlendMapTexture();
				for (int i = 0; i < smoothness_iter; ++i)	estimator.smoothness();
				estimator.generateHeightMap("../resources/texture/terrain/height16bit2.png", width, height);
				estimator.generateBlendMap("../resources/texture/terrain/splatMap.png", width, height);
				estimator.initHMapData(frameTexture, 2048, 2048);
			}
			ImGui::SameLine();
			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		ImGui::TextWrapped("Export height-map and blend-map as resources which can be used in unreal engine editor");
		if (ImGui::Button("Export UnrealEngine.."))
		{
			auto& estimator = Estimator::getMutableInstance();
			estimator.initHMapData(frameTexture, 2048, 2048);
			ImGui::OpenPopup("Export UnrealEngine");
		}
		if (ImGui::BeginPopupModal("Export UnrealEngine"))
		{

			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
	}
	
	ImGui::End();
}

void NoiseGUI::renderGUI(void) const
{
	ImGui::Render();
}

void NoiseGUI::StyleColorsDark(ImGuiStyle* dst)
{
	ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
}