#include "NoiseGUI.hpp"
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <imgui/imgui_internal.h>
#include "GLShader.hpp"
#include <vector>

using namespace ImGui;

#include <opencv2/imgcodecs.hpp>
using namespace cv;

bool NoiseGUI::isInstanciated = false;

NoiseGUI::NoiseGUI()
	: isGUIOpen(true), isSaveButtonPushed(false)
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

	ImGui::End();
}

void NoiseGUI::renderGUI(void) const
{
	ImGui::Render();
}


bool NoiseGUI::saveCurrentTexture(const std::string& path, int width, int height, uint32_t texture)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	std::vector<unsigned short> data(width * height);

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_SHORT, (void*)&data[0]);
	FILE *arrOut = fopen("outHeight.txt", "w");
	fprintf(arrOut, "%d %d\n", height, width);
	int cnt = 0;
	for (const auto p : data) {
		if (cnt == 0) fprintf(arrOut, "%d ", p);
		cnt = (cnt + 1) % 3;
	}
	fclose(arrOut);

	Mat src = Mat(cv::Size(width, height), CV_16UC1, (void*)&data[0]);
	imwrite(path, src);

	glBindTexture(GL_TEXTURE_2D, 0);
	
	return true;
}