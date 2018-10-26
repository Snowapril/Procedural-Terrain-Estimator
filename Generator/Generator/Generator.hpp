#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "GLApp.hpp"
#include <glm/vec2.hpp>
#include <vector>
#include <stdint.h>
#include <array>

class NoiseGUI;
class GLShader;
class BrushBoard;

class Generator : public GLApp
{
private:
	uint32_t VAO;
	uint32_t VBO;
	uint32_t framebuffer;
	uint32_t framebufferTexture;
	
	std::shared_ptr<GLShader> generatorShader;
	std::unique_ptr<GLShader> screenShader;
	std::unique_ptr<NoiseGUI> noiseGui;

	std::array<std::shared_ptr<BrushBoard>, 3> paintBoards;
protected:
	void updateScene(void);
	void drawScene(void) const;
	void onResize(int newWidth, int newHeight);

	bool initFramebuffer(int width, int height);
	bool initShaders(void);

	bool saveCurrentTexture(const std::string& path, int width, int height) const;
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