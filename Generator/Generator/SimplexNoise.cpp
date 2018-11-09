#include "SimplexNoise.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <imgui/imgui_internal.h>
using namespace ImGui;

#include "GLShader.hpp"

SimplexNoise::SimplexNoise()
	: blend(1.0f), frequency(2.0f)
{
}

SimplexNoise::SimplexNoise(const Util::Rect & rect)
	: BrushBoard(rect), blend(1.0f), frequency(2.0f)
{
}

void SimplexNoise::updateGUI(void)
{
	if (ImGui::TreeNode("Simplex Noise"))
	{
		ImGui::SliderFloat("Blend", &blend, 0.0f, 1.0f);
		ImGui::SliderFloat("Frequency", &frequency, 0.0f, 30.0f);

		ImGui::TreePop();
	}
}

void SimplexNoise::sendProperties(std::shared_ptr<GLShader> shader)
{
	shader->useProgram();

	shader->sendUniform("simplex.blend", blend);
	shader->sendUniform("simplex.frequency", frequency);
	shader->sendUniform("simplex.viewPoint", getViewPoint());
}