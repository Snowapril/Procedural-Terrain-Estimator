#include "fbMNoise.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <imgui/imgui_internal.h>
using namespace ImGui;

#include "GLShader.hpp"

FBMNoise::FBMNoise()
	: blend(1.0f), frequency(2.0f), numOctaves(5)
{
}

FBMNoise::FBMNoise(const Util::Rect & rect)
	: BrushBoard(rect), blend(1.0f), frequency(2.0f), numOctaves(5)
{
}

void FBMNoise::updateGUI(void)
{
	if (ImGui::TreeNode("fbM Noise"))
	{
		ImGui::SliderInt("Num Octaves", &numOctaves, 1, 5);
		ImGui::SliderFloat("Blend", &blend, 0.0f, 1.0f);
		ImGui::SliderFloat("Frequency", &frequency, 0.0f, 30.0f);

		ImGui::TreePop();
	}
}

void FBMNoise::sendProperties(std::shared_ptr<GLShader> shader)
{
	shader->useProgram();

	shader->sendUniform("fbM.num_octaves", numOctaves);
	shader->sendUniform("fbM.blend", blend);
	shader->sendUniform("fbM.frequency", frequency);
	shader->sendUniform("fbM.viewPoint", getViewPoint());
}
