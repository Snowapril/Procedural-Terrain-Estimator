#include "fbMNoise.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <imgui/imgui_internal.h>
using namespace ImGui;

#include "obfuscator.hpp"
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
	if (ImGui::TreeNode(OBFUSCATE("fbM Noise")))
	{
		const char* items[] = { "1 step", "2 step", "3 step", "4 step", "5 step" };
		static int octavesIdx = 4;

		ImGui::PushItemWidth(210);
		ImGui::SliderFloat(OBFUSCATE("Blend"), &blend, 0.0f, 1.0f);
		ImGui::ListBox(OBFUSCATE("Octaves"), &octavesIdx, items, IM_ARRAYSIZE(items));
		numOctaves = octavesIdx + 1;
		ImGui::SliderFloat(OBFUSCATE("Frequency"), &frequency, 0.0f, 15.0f);
		ImGui::PopItemWidth();

		ImGui::TreePop();
	}
}

void FBMNoise::sendProperties(std::shared_ptr<GLShader> shader)
{
	shader->useProgram();

	shader->sendUniform(OBFUSCATE("fbM.num_octaves"), numOctaves);
	shader->sendUniform(OBFUSCATE("fbM.blend"), blend);
	shader->sendUniform(OBFUSCATE("fbM.frequency"), frequency);
	shader->sendUniform(OBFUSCATE("fbM.viewPoint"), getViewPoint());
}
