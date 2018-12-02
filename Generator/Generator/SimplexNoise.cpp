#include "SimplexNoise.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <imgui/imgui_internal.h>
using namespace ImGui;

#include "GLShader.hpp"
#include "obfuscator.hpp"

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
	if (ImGui::TreeNode(OBFUSCATE("Simplex Noise")))
	{
		ImGui::PushItemWidth(210);
		ImGui::SliderFloat(OBFUSCATE("Blend"), &blend, 0.0f, 1.0f);
		ImGui::SliderFloat(OBFUSCATE("Frequency"), &frequency, 0.0f, 15.0f);
		ImGui::PopItemWidth();

		ImGui::TreePop();
	}
}

void SimplexNoise::sendProperties(std::shared_ptr<GLShader> shader)
{
	shader->useProgram();

	shader->sendUniform(OBFUSCATE("simplex.blend"), blend);
	shader->sendUniform(OBFUSCATE("simplex.frequency"), frequency);
	shader->sendUniform(OBFUSCATE("simplex.viewPoint"), getViewPoint());
}