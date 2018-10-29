#include "NoiseGUI.hpp"
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <imgui/imgui_internal.h>
#include "GLShader.hpp"
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

using namespace ImGui;

bool NoiseGUI::isInstanciated = false;

NoiseGUI::NoiseGUI()
	: isGUIOpen(true), isSaveButtonPushed(false)
{
	assert(!isInstanciated);
	isInstanciated = true;

	voronoiConfigure = { 1.0f, 2.0f, 0.0f, 0.0f, false, false };
	simplexConfigure = { 1.0f, 2.0f };
	fbMConfigure	 = { 5, 1.0f, 2.0f };
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

void NoiseGUI::processCursorPos(double xpos, double ypos)
{
}

void NoiseGUI::processWheelOffset(double yoffset)
{
}

void NoiseGUI::processMouseBtn(int button, int action)
{
}


void NoiseGUI::processToggleKey(int key, int scancode, int action)
{
}

void NoiseGUI::updateGUI(float height, uint32_t frameTexture, uint32_t activeBoardIndex)
{
	ImGui_ImplGlfwGL3_NewFrame();

	ImGui::Begin("Generator & Estimator", &isGUIOpen, ImVec2(0, 0), 0.5f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize(ImVec2(400, height));

	if (ImGui::TreeNode("Simplex Noise"))
	{
		ImGui::SliderFloat("Blend", &simplexConfigure.blend, 0.0f, 1.0f);
		ImGui::SliderFloat("Frequency", &simplexConfigure.frequency, 0.0f, 30.0f);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Voronoi Noise"))
	{
		ImGui::SliderFloat("Blend", &voronoiConfigure.blend, 0.0f, 1.0f);
		ImGui::SliderFloat("Frequency", &voronoiConfigure.frequency, 0.0f, 30.0f);
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
		ImGui::SliderFloat("Frequency", &fbMConfigure.frequency, 0.0f, 30.0f);

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
		saveCurrentTexture("../resources/noiseMap.png", 2048, 2048, frameTexture);
	}

	ImGui::End();
}

void NoiseGUI::renderGUI(void) const
{
	ImGui::Render();
}

void NoiseGUI::sendProperties(std::shared_ptr<GLShader> shader)
{
	shader->useProgram();

	shader->sendUniform("simplex.blend", simplexConfigure.blend);
	shader->sendUniform("simplex.frequency", simplexConfigure.frequency);

	shader->sendUniform("voronoi.blend", voronoiConfigure.blend);
	shader->sendUniform("voronoi.frequency", voronoiConfigure.frequency);
	shader->sendUniform("voronoi.function", voronoiConfigure.function);
	shader->sendUniform("voronoi.distance_type", voronoiConfigure.distance_type);
	shader->sendUniform("voronoi.multiply_by_F1", voronoiConfigure.multiply_by_F1);
	shader->sendUniform("voronoi.inverse", voronoiConfigure.inverse);

	shader->sendUniform("fbM.num_octaves", fbMConfigure.numOctaves);
	shader->sendUniform("fbM.blend", fbMConfigure.blend);
	shader->sendUniform("fbM.frequency", fbMConfigure.frequency);
}

bool NoiseGUI::saveCurrentTexture(const std::string& path, int width, int height, uint32_t texture)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	std::vector<unsigned char> data(width * height * 3);

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)&data[0]);
	FILE *arrOut = fopen("outHeight.txt", "w");
	fprintf(arrOut, "%d %d\n", height, width);
	int cnt = 0;
	for (const auto p : data) {
		if (cnt == 0) fprintf(arrOut, "%d ", p);
		cnt = (cnt + 1) % 3;
	}
	fclose(arrOut);
	stbi_write_png(path.c_str(), height, width, 3, &data[0], 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}
