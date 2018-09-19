#include "Generator.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <imgui/imgui_internal.h>
#include "EngineProperty.hpp"

#include "GLDebugger.hpp"

using namespace ImGui;

Generator::Generator()
	: isGUIOpen(true)
{
}

Generator::~Generator()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteTextures(1, &framebufferTexture);
	glDeleteFramebuffers(1, &framebuffer);
	ImGui_ImplGlfwGL3_Shutdown();
}

void Generator::updateScene(void)
{
	updateGUI(clientHeight);
}

void Generator::drawScene(void) const
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(Color::Black[0], Color::Black[1], Color::Black[2], Color::Black[3]);

	glViewport(0, 0, 400, clientHeight);
	ImGui::Render();

	glViewport(400, 0, clientWidth - 400, clientHeight);
}

void Generator::onResize(int newWidth, int newHeight)
{
	GLApp::onResize(newWidth, newHeight);
}

bool Generator::initGUI(void)
{
	if (!ImGui_ImplGlfwGL3_Init(window, true))
		return false;

	GLfloat vertices[] = {
		-1.0f,  1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)(sizeof(GLfloat) * 2));
	glBindVertexArray(0);

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &framebufferTexture);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, clientWidth - 400, clientHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	CheckError();
	return true;
}

void Generator::updateGUI(float height)
{
	ImGui_ImplGlfwGL3_NewFrame();
	
	ImGui::Begin("Generator & Estimator", &isGUIOpen, ImVec2(0, 0), 0.5f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize(ImVec2(400, height));

	if (ImGui::TreeNode("Perlin Noise"))
	{
		float blend = 0.0f;

		ImGui::SliderFloat("Blend", &blend, 0.0f, 1.0f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Simplex Noise"))
	{
		float blend = 0.0f;

		ImGui::SliderFloat("Blend", &blend, 0.0f, 1.0f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Voronoi Noise"))
	{
		float blend = 0.0f;

		ImGui::SliderFloat("Blend", &blend, 0.0f, 1.0f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("fbM Noise"))
	{
		float blend = 0.0f;

		ImGui::SliderFloat("Blend", &blend, 0.0f, 1.0f);

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

bool Generator::saveCurrentTexture(const std::string& path)
{

}