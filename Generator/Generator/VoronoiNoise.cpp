#include "VoronoiNoise.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <imgui/imgui_internal.h>
using namespace ImGui;

#include "GLShader.hpp"
#include "obfuscator.hpp"

VoronoiNoise::VoronoiNoise()
	: blend(1.0f), frequency(2.0f), function(0.0f), distance_type(0.0f), multiply_by_F1(false), inverse(false)
{
}

VoronoiNoise::VoronoiNoise(const Util::Rect & rect)
	: BrushBoard(rect), blend(1.0f), frequency(2.0f), function(0.0f), distance_type(0.0f), multiply_by_F1(false), inverse(false)
{
}

void VoronoiNoise::updateGUI(void)
{
	if (ImGui::TreeNode(OBFUSCATE("Voronoi Noise")))
	{
		ImGui::SliderFloat(OBFUSCATE("Blend"), &blend, 0.0f, 1.0f);
		ImGui::SliderFloat(OBFUSCATE("Frequency"), &frequency, 0.0f, 30.0f);
		ImGui::SliderFloat(OBFUSCATE("Function"), &function, 0.5f, 3.5f);
		ImGui::SliderFloat(OBFUSCATE("Distance Type"), &distance_type, 0.5f, 3.5f);
		ImGui::Checkbox(OBFUSCATE("Multiply"), &multiply_by_F1);
		ImGui::Checkbox(OBFUSCATE("Inverse"), &inverse);

		ImGui::TreePop();
	}
}

void VoronoiNoise::sendProperties(std::shared_ptr<GLShader> shader)
{
	shader->useProgram();

	shader->sendUniform(OBFUSCATE("voronoi.blend"), blend);
	shader->sendUniform(OBFUSCATE("voronoi.frequency"), frequency);
	shader->sendUniform(OBFUSCATE("voronoi.function"), function);
	shader->sendUniform(OBFUSCATE("voronoi.distance_type"), distance_type);
	shader->sendUniform(OBFUSCATE("voronoi.multiply_by_F1"), multiply_by_F1);
	shader->sendUniform(OBFUSCATE("voronoi.inverse"), inverse);
	shader->sendUniform(OBFUSCATE("voronoi.viewPoint"), getViewPoint());
}
