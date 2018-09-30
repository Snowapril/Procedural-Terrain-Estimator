#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "GLApp.hpp"
#include "NoiseGUI.hpp"
#include <glm/vec2.hpp>
#include <vector>
#include <stdint.h>

class GLShader;

class Generator : public GLApp, NoiseGUI
{
private:
	bool isScreenClicked;
	
	uint32_t VAO;
	uint32_t VBO;
	uint32_t framebuffer;
	uint32_t framebufferTexture;
	uint32_t brushTexture;

	std::vector<float> brushFilter;
	std::vector<float> brushBoard;

	std::unique_ptr<GLShader> generatorShader;
	std::unique_ptr<GLShader> screenShader;
protected:
	void updateScene(void);
	void drawScene(void) const;
	void onResize(int newWidth, int newHeight);

	bool initFramebuffer(int width, int height);
	bool initShaders(void);

	bool saveCurrentTexture(const std::string& path, int width, int height);

	void updateBrushTexture(void);
	void applyBrush(double xoffset, double yoffset);
public:
	Generator();
	virtual ~Generator();
public:
	bool initGenerator(void);

	void keyCallback(int key, int scancode, int action, int mode);
	void mousePosCallback(double xpos, double ypos);
	void mouseBtnCallback(int btn, int action, int mods);
	void scrollCallback(double xoffset, double yoffset);
};

#endif