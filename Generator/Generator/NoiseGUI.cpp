#include "NoiseGUI.hpp"
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <imgui/imgui_internal.h>

using namespace ImGui;

NoiseGUI::NoiseGUI()
	: isGUIOpen(true), isSaveButtonPushed(false)
{
	voronoiConfigure = { 1.0f, 2.0f, 0.0f, 0.0f, false, false };
	simplexConfigure = { 0.0f, 2.0f };
	fbMConfigure	 = { 5, 0.0f, 2.0f };
}

NoiseGUI::~NoiseGUI()
{
	ImGui_ImplGlfwGL3_Shutdown();
}

bool NoiseGUI::initGUI(GLFWwindow* window)
{
	if (!ImGui_ImplGlfwGL3_Init(window, true))
		return false;

	return true;
}

void NoiseGUI::updateGUI(float height)
{
	ImGui_ImplGlfwGL3_NewFrame();

	ImGui::Begin("Generator & Estimator", &isGUIOpen, ImVec2(0, 0), 0.5f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize(ImVec2(400, height));

	if (ImGui::TreeNode("Simplex Noise"))
	{
		ImGui::SliderFloat("Blend", &simplexConfigure.blend, 0.0f, 1.0f);
		ImGui::SliderFloat("Frequency", &simplexConfigure.frequency, 1.0f, 30.0f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Voronoi Noise"))
	{
		ImGui::SliderFloat("Blend", &voronoiConfigure.blend, 0.0f, 1.0f);
		ImGui::SliderFloat("Frequency", &voronoiConfigure.frequency, 1.0f, 30.0f);
		ImGui::SliderFloat("Function", &voronoiConfigure.function, 0.5f, 3.5f);
		ImGui::SliderFloat("Distance Type", &voronoiConfigure.distance_type, 0.5f, 3.5f);
		ImGui::Checkbox("Multiply", &voronoiConfigure.multiply_by_F1);
		ImGui::Checkbox("Inverse", &voronoiConfigure.inverse);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("fbM Noise"))
	{
		ImGui::SliderInt("Num Octaves", &fbMConfigure.numOctaves, 1, 5);
		ImGui::SliderFloat("Blend", &fbMConfigure.blend, 0.0f, 1.0f);
		ImGui::SliderFloat("Frequency", &fbMConfigure.frequency, 1.0f, 30.0f);

		ImGui::TreePop();
	}

	if (ImGui::CollapsingHeader("Key Support", 0, true, true))
	{
		ImGui::Text("ESC   : Quit Program");
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
		isSaveButtonPushed = true;
	}
	else
		isSaveButtonPushed = false;

	ImGui::End();
}

void NoiseGUI::rengerGUI(void) const
{
	ImGui::Render();
}