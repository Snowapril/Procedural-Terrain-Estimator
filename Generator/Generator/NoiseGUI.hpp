#ifndef NOISE_GUI_HPP
#define NOISE_GUI_HPP

#include "EventHandler.hpp"
#include <memory>
#include <string>
#include "NonCopyable.hpp"
#include <glm/vec2.hpp>

class GLShader;
struct GLFWwindow;

class NoiseGUI : public NonCopyable
{
protected:
	static bool isInstanciated;
	bool isGUIOpen;
	bool isSaveButtonPushed;

public:
	void updateGUI(glm::vec2 rect);
	void endUpdate(uint32_t frameTexture);
	void renderGUI(void) const;

public:
	NoiseGUI();
	~NoiseGUI();
public:
	bool initGUI(GLFWwindow* window);

	static void saveCurrentTexture(const std::string& path, int width, int height, uint32_t texture);
};

#endif