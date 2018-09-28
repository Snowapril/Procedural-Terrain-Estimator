#include "Generator.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <imgui/imgui_internal.h>
#include "EngineProperty.hpp"
#include <vector>
#include "GLDebugger.hpp"
//#include "Estimator.hpp"
#include "GLShader.hpp"
#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

using namespace ImGui;

Generator::Generator()
	: isGUIOpen(true)
{
	voronoiConfigure = { 1.0f, 2.0f, 0.0f, 0.0f, false, false };
	simplexConfigure = { 0.0f, 2.0f };
	fbMConfigure	 = { 5, 0.0f, 2.0f };
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

	generatorShader->useProgram();
	generatorShader->sendUniform("systemTime", static_cast<float>(glfwGetTime()));

	generatorShader->sendUniform("simplex.blend", simplexConfigure.blend);
	generatorShader->sendUniform("simplex.frequency", simplexConfigure.frequency);

	generatorShader->sendUniform("voronoi.blend", voronoiConfigure.blend);
	generatorShader->sendUniform("voronoi.frequency", voronoiConfigure.frequency);
	generatorShader->sendUniform("voronoi.function", voronoiConfigure.function);
	generatorShader->sendUniform("voronoi.distance_type", voronoiConfigure.distance_type);
	generatorShader->sendUniform("voronoi.multiply_by_F1", voronoiConfigure.multiply_by_F1);
	generatorShader->sendUniform("voronoi.inverse", voronoiConfigure.inverse);

	generatorShader->sendUniform("fbM.num_octaves", fbMConfigure.numOctaves);
	generatorShader->sendUniform("fbM.blend", fbMConfigure.blend);
	generatorShader->sendUniform("fbM.frequency", fbMConfigure.frequency);
}

void Generator::drawScene(void) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(Color::Black[0], Color::Black[1], Color::Black[2], Color::Black[3]);
	glViewport(0, 0, clientWidth, clientHeight);

	generatorShader->useProgram();
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(Color::Black[0], Color::Black[1], Color::Black[2], Color::Black[3]);
	glViewport(400, 0, clientWidth - 400, clientHeight);
	screenShader->useProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glViewport(0, 0, 400, clientHeight);
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
	
	ImGui::Begin("Generator & Estimator", &isGUIOpen, ImVec2(0, 0), 0.5f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize(ImVec2(400, height));

	if (ImGui::TreeNode("Perlin Noise"))
	{
		
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Simplex Noise"))
	{
		ImGui::SliderFloat("Blend", &simplexConfigure.blend, 0.0f, 1.0f);
		ImGui::SliderFloat("Frequency", &simplexConfigure.frequency, 1.0f, 30.0f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Voronoi Noise"))
	{
		ImGui::SliderFloat("Blend", &voronoiConfigure.blend, 0.0f, 1.0f);
		ImGui::SliderFloat("Frequency", &voronoiConfigure.frequency, 1.0f, 30.0f);
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
		ImGui::SliderFloat("Frequency", &fbMConfigure.frequency, 1.0f, 30.0f);

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
		saveCurrentTexture("../resources/noiseMap.png", clientWidth - 400, clientHeight);
	}

	ImGui::End();
}

bool Generator::initShaders(void)
{
	try
	{
		generatorShader = std::make_unique<GLShader>(
			"../resources/shader/generator_vs.glsl", 
			"../resources/shader/generator_fs.glsl"
		);
		screenShader = std::make_unique<GLShader>(
			"../resources/shader/offscreen_vs.glsl",
			"../resources/shader/offscreen_fs.glsl"
		);
	}
	catch (std::exception e)
	{
		std::cerr << "Initializing Shader Failed" << std::endl;
		return false;
	}

	screenShader->useProgram();
	screenShader->sendUniform("framebufferTexture", 0);
	
	CheckError();

	return true;
}

bool Generator::initFramebuffer(int width, int height)
{
	GLfloat vertices[] = {
			-1.0f, -1.0f, 0.0f, 0.0f,
			-1.0f,  1.0f, 0.0f, 1.0f,
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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
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

bool Generator::initGenerator(void)
{
	if (!GLApp::initGLApp())
		return false;

	if (!initGUI())
		return false;
	
	if (!initFramebuffer(clientWidth - 400, clientHeight))
		return false;

	if (!initShaders())
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

bool Generator::saveCurrentTexture(const std::string& path, int width, int height)
{
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	std::vector<unsigned char> data(width * height * 3);

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)&data[0]);
	FILE *arrOut = fopen("../Mapdata.pte", "w");
	fprintf(arrOut, "%d %d\n", height, width);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			fprintf(arrOut,"%d ", data[i*width + j * 3]);
		}
		fprintf(arrOut,"\n");
	}
	fclose(arrOut);

	//Estimator::mapDataInit(data, height, width);
	//Estimator::dumpMapData(height, width);

	stbi_write_png(path.c_str(), height, width, 3, &data[0], 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}