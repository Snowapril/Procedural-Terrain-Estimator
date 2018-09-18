#include "Generator.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <imgui/imgui_internal.h>

using namespace ImGui;

Generator::Generator()
	: isGUIOpen(true)
{
}

Generator::~Generator()
{
	ImGui_ImplGlfwGL3_Shutdown();
}

void Generator::updateScene(void)
{
	updateGUI(clientHeight);
}

void Generator::drawScene(void) const
{
	ImGui::Render();
}

void Generator::onResize(int newWidth, int newHeight)
{
	GLApp::onResize(newWidth, newHeight);
}

bool Generator::initGUI(void)
{
	if (!ImGui_ImplGlfwGL3_Init(window, true))
		return false;

	return true;
}

void Generator::updateGUI(float height)
{
	ImGui_ImplGlfwGL3_NewFrame();

	ImGui::Begin("Generator & Estimator", &isGUIOpen, ImVec2(0, 0), 0.5f, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize(ImVec2(400, height));

	if (ImGui::CollapsingHeader("Noise Blending", 0, true, true))
	{
		float a = 0.0f, b = 0.0f, c = 0.0f, d = 0.0f;
		
		ImGui::SliderFloat("Perlin Noise", &a, 0.0f, 1.0f);
		ImGui::SliderFloat("Simplex Noise", &b, 0.0f, 1.0f);
		ImGui::SliderFloat("Voronoi Noise", &c, 0.0f, 1.0f);
		ImGui::SliderFloat("fbM Noise", &d, 0.0f, 1.0f);

		if (ImGui::Button("Set"))
		{
			
		}
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
		ImGui::Text("Map Generator & Estimator by kkorona");
		ImGui::Text("Email: high1uck@pusan.ac.kr");
	}

	ImGui::End();
}

bool Generator::initGenerator(void)
{
	if (!GLApp::initGLApp())
		return false;

	if (!initGUI())
		return false;

	return true;
}

void Generator::keyCallback(int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void Generator::mousePosCallback(double xpos, double ypos)
{
}

void Generator::mouseBtnCallback(int btn, int action, int mods)
{
}

void Generator::scrollCallback(double xoffset, double yoffset)
{
}
