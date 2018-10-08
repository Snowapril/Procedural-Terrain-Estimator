#include "Generator.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <imgui/imgui_internal.h>
#include "EngineProperty.hpp"
#include <vector>
#include "GLDebugger.hpp"
#include "GLShader.hpp"
#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

using namespace ImGui;

constexpr uint32_t BRUSH_WIDTH = 50;
constexpr uint32_t BRUSH_HEIGHT = 50;

constexpr uint32_t HALF_BRUSH_WIDTH = BRUSH_WIDTH / 2;
constexpr uint32_t HALF_BRUSH_HEIGHT = BRUSH_HEIGHT / 2;

constexpr float BRUSH_DECAL = 0.002f;

Generator::Generator()
	: isScreenClicked(false)
{
}

Generator::~Generator()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteTextures(1, &framebufferTexture);
	glDeleteFramebuffers(1, &framebuffer);
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


	if (isScreenClicked)
	{
		updateBrushTexture();
	}
}

void Generator::drawScene(void) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(Color::Black[0], Color::Black[1], Color::Black[2], Color::Black[3]);
	glViewport(0, 0, 2048, 2048);

	generatorShader->useProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, brushTexture);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	if (isSaveButtonPushed)
		saveCurrentTexture("../resources/noiseMap.png", 2048, 2048);

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

	generatorShader->useProgram();
	generatorShader->sendUniform("brushBoard", 0);

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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1, &brushTexture);
	glBindTexture(GL_TEXTURE_2D, brushTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, clientWidth, clientHeight, 0, GL_RED, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	brushBoard.resize(clientWidth * clientHeight, 0);
	
	brushFilter.resize(BRUSH_WIDTH * BRUSH_HEIGHT);

	constexpr int centerX = BRUSH_WIDTH / 2;
	constexpr int centerY = BRUSH_HEIGHT / 2;

	for (int r = 0; r < BRUSH_WIDTH; ++r)
	{
		for (int c = 0; c < BRUSH_HEIGHT; ++c)
		{
			float distance = std::sqrt((centerX - r) * (centerX - r) + (centerY - c) * (centerY - c));
			
			brushFilter[r * BRUSH_HEIGHT + c] = std::exp(-distance * distance * BRUSH_DECAL);
		}
	}

	CheckError();

	return true;
}

bool Generator::initGenerator(void)
{
	if (!GLApp::initGLApp())
		return false;

	if (!initGUI(window))
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

	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
	{
		std::fill(brushBoard.begin(), brushBoard.end(), 0.0f);
		updateBrushTexture();
	}
}

void Generator::mousePosCallback(double xpos, double ypos)
{
	if (isScreenClicked && 400.0 <= xpos && xpos < clientWidth && ypos >= 0.0 && ypos < clientHeight)
		applyBrush(xpos - 400, clientHeight - ypos);
}

void Generator::mouseBtnCallback(int btn, int action, int mods)
{
	if (btn == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		isScreenClicked = true;
	if (btn == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		isScreenClicked = false;
}

void Generator::scrollCallback(double xoffset, double yoffset)
{
}

void Generator::updateBrushTexture(void)
{
	glBindTexture(GL_TEXTURE_2D, brushTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, clientWidth, clientHeight, GL_RED, GL_FLOAT, &brushBoard[0]);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool isInRange(int width, int height, int i, int j)
{
	return i >= 0 && i < width && j >= 0 && j < height;
}

void Generator::applyBrush(double xoffset, double yoffset)
{
	//TODO : main target of performance upgrade
	for (int r = 0; r < BRUSH_WIDTH; ++r)
	{
		for (int c = 0; c < BRUSH_HEIGHT; ++c)
		{
			if (isInRange(clientWidth, clientHeight, r - HALF_BRUSH_WIDTH + yoffset, c - HALF_BRUSH_HEIGHT + xoffset))
			{
				const uint32_t idx = (r - HALF_BRUSH_WIDTH + yoffset) * clientWidth + (c - HALF_BRUSH_HEIGHT + xoffset);
				brushBoard[idx] += brushFilter[r * BRUSH_HEIGHT + c];

				brushBoard[idx] = brushBoard[idx] > 1.0f ? 1.0f : brushBoard[idx];
			}
		}
	}
}

bool Generator::saveCurrentTexture(const std::string& path, int width, int height) const
{
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	std::vector<unsigned char> data(width * height * 3);

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)&data[0]);
	FILE *arrOut = fopen("outHeight.txt", "w");
	fprintf(arrOut, "%d %d\n", height, width);
	int cnt = 0;
	for (const auto p : data) {
		if (cnt == 0) fprintf(arrOut, "%d ", p);
		cnt = (cnt+1)%3;
	}		
	fclose(arrOut);
	stbi_write_png(path.c_str(), height, width, 3, &data[0], 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}