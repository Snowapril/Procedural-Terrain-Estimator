#include "VoronoiNoise.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <imgui/imgui_internal.h>
using namespace ImGui;

#include "GLShader.hpp"
#include "obfuscator.hpp"

VoronoiNoise::VoronoiNoise()
	: blend(1.0f), frequency(2.0f), function(0), distance_type(0), multiply_by_F1(false), inverse(false)
{
}

VoronoiNoise::VoronoiNoise(const Util::Rect & rect)
	: BrushBoard(rect), blend(1.0f), frequency(2.0f), function(0), distance_type(0), multiply_by_F1(false), inverse(false)
{
}

void VoronoiNoise::updateGUI(void)
{
	if (ImGui::TreeNode(OBFUSCATE("Voronoi Noise")))
	{
		ImGui::PushItemWidth(210);
		ImGui::SliderFloat(OBFUSCATE("Blend"), &blend, 0.0f, 1.0f);
		ImGui::SliderFloat(OBFUSCATE("Frequency"), &frequency, 0.0f, 15.0f);
		ImGui::Text(OBFUSCATE("Function"));
		ImGui::RadioButton("f_1", &function, 0); ImGui::SameLine();
		ImGui::RadioButton("f_2", &function, 1); ImGui::SameLine();
		ImGui::RadioButton("f_3", &function, 2); ImGui::SameLine();
		ImGui::RadioButton("f_4", &function, 3); 
		ImGui::Text(OBFUSCATE("Distance Type"));
		ImGui::RadioButton("d_1", &distance_type, 0); ImGui::SameLine();
		ImGui::RadioButton("d_2", &distance_type, 1); ImGui::SameLine();
		ImGui::RadioButton("d_3", &distance_type, 2); ImGui::SameLine();
		ImGui::RadioButton("d_4", &distance_type, 3);
		ImGui::Checkbox(OBFUSCATE("Multiply"), &multiply_by_F1); ImGui::SameLine(150);
		ImGui::Checkbox(OBFUSCATE("Inverse"), &inverse);
		ImGui::PopItemWidth();

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
