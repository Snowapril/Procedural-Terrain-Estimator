#include "EngineGUI.hpp"
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <imgui/imgui_internal.h>

using namespace ImGui;

enum class RenderMode : int
{
	
};

EngineGUI::EngineGUI()
	: isGUIOpen(true)
{

}

EngineGUI::~EngineGUI()
{
	ImGui_ImplGlfwGL3_Shutdown();
}


bool EngineGUI::initGUI(GLFWwindow* window)
{
	if (!ImGui_ImplGlfwGL3_Init(window, true))
		return false;
	
	StyleColorsDark(&GetStyle());

	return true;
}

void EngineGUI::startUpdateGUI(float dt, float height)
{
	ImGui_ImplGlfwGL3_NewFrame();

	ImGui::Begin("Terrain Renderer", &isGUIOpen, ImVec2(0, 0), 0.5f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize(ImVec2(450, height));
}

void EngineGUI::endUpdateGUI(float dt)
{
	if (ImGui::CollapsingHeader("Detail About Program"))
	{
		ImGui::Text("ESC   : Quit Program");
		ImGui::Text("RMB   : Zoom in/out Automatically");
		ImGui::Text("Wheel : Zoom in/out");
		ImGui::Text("Delta time %.2f ms", dt);

		char* glInfos = (char*)glGetString(GL_VERSION);
		char* hardwareInfos = (char*)glGetString(GL_RENDERER);

		ImGui::Text("OpenGL Version :");
		ImGui::Text(glInfos);
		ImGui::Text("Hardware Informations :");
		ImGui::Text(hardwareInfos);
	}
	
	ImGui::Text("\nFramerate %.2f FPS / Frametime %.4f ms", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("Procedrual Terrain Estimator");
	ImGui::Text("Terrain Renderer by Snowapril");
	ImGui::Text("Email: sinjihng@pusan.ac.kr");

	ImGui::End();
}

void EngineGUI::renderGUI(void) const
{
	Render();
}

void EngineGUI::StyleColorsDark(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
}