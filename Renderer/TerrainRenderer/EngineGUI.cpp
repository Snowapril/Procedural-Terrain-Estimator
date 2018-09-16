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

	return true;
}

void EngineGUI::updateGUI(float dt, float height)
{
	ImGui_ImplGlfwGL3_NewFrame();

	ImGui::Begin("Terrain Renderer", &isGUIOpen, ImVec2(0, 0), 0.5f, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize(ImVec2(300, height));

	//GUI setup code here.
	if (ImGui::TreeNode("Post Processing"))
	{
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Camera"))
	{
		ImGui::TreePop();
	}

	if (ImGui::CollapsingHeader("Key Support", 0, true, true))
	{
		ImGui::Text("ESC   : Quit Program");
		ImGui::Text("RMB   : Zoom in/out Automatically");
		ImGui::Text("Wheel : Zoom in/out");
	}

	if (ImGui::CollapsingHeader("Profiling", 0, true, true))
	{
		ImGui::Text("Delta time %.2f ms", dt);
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
		ImGui::Text("Terrain Renderer by Snowapril");
		ImGui::Text("Email: sinjihng@naver.com");
	}

	ImGui::End();
}

void EngineGUI::renderGUI(void) const
{
	Render();
}
