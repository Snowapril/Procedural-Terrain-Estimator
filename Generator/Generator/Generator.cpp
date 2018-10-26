#include "Generator.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "EngineProperty.hpp"
#include <vector>
#include "GLDebugger.hpp"
#include "GLShader.hpp"
#include <iostream>
#include "BrushBoard.hpp"
#include "NoiseGUI.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>


Generator::Generator()
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
	noiseGui->updateGUI(clientHeight);

	noiseGui->sendProperties(generatorShader);

	for (size_t i = 0; i < 3; ++i)
	{
		paintBoards[i]->updateBrushTexture(10U);	
	}
}

void Generator::drawScene(void) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(Color::Black[0], Color::Black[1], Color::Black[2], Color::Black[3]);
	glViewport(0, 0, 2048, 2048);

	generatorShader->useProgram();
	for (size_t i = 0; i < 3; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, paintBoards[i]->getBrushTexture());
	}

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//if (isSaveButtonPushed)
	//	saveCurrentTexture("../resources/noiseMap.png", 2048, 2048);
	
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
	
	noiseGui->renderGUI();
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
	generatorShader->sendUniform("voronoiBoard", 0);
	generatorShader->sendUniform("simplexBoard", 1);
	generatorShader->sendUniform("fbMBoard", 2);

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

	CheckError();

	return true;
}

bool Generator::initGenerator(void)
{
	if (!GLApp::initGLApp())
		return false;

	noiseGui = std::make_unique<NoiseGUI>();
	if (!noiseGui->initGUI(window))
		return false;
	
	if (!initFramebuffer(clientWidth - 400, clientHeight))
		return false;

	if (!initShaders())
		return false;

	for (size_t i = 0; i < 3; ++i)
	{
		paintBoards[i] = std::make_shared<BrushBoard>(2048, 2048);
	}

	return true;
}

void Generator::keyCallback(int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void Generator::mousePosCallback(double xpos, double ypos)
{
	for (const auto& board : paintBoards)
		board->processCursorPos(xpos, ypos);
}

void Generator::mouseBtnCallback(int btn, int action, int mods)
{
	for (const auto& board : paintBoards)
		board->processMouseBtn(btn, action);
}

void Generator::scrollCallback(double xoffset, double yoffset)
{
	for (const auto& board : paintBoards)
		board->processWheelOffset(yoffset);
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