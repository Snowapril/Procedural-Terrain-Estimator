#ifndef NOISE_GUI_HPP
#define NOISE_GUI_HPP

#include "EventHandler.hpp"
#include <memory>
#include <string>

class GLShader;
struct GLFWwindow;

class NoiseGUI : public EventHandler
{
protected:
	static bool isInstanciated;
	bool isGUIOpen;
	bool isSaveButtonPushed;

	struct Voronoi {
		float blend;
		float frequency;
		float function;
		float distance_type;
		bool multiply_by_F1;
		bool inverse;
	} voronoiConfigure;

	struct Simplex {
		float blend;
		float frequency;
	} simplexConfigure;

	struct FBM {
		int numOctaves;
		float blend;
		float frequency;
	} fbMConfigure;

public:
	void updateGUI(float height, uint32_t frameTexture);
	void sendProperties(std::shared_ptr<GLShader> shader);
	void renderGUI(void) const;

public:
	NoiseGUI();
	NoiseGUI(const NoiseGUI& other) = delete;
	NoiseGUI& operator=(const NoiseGUI& other) = delete;
	~NoiseGUI();
public:
	bool initGUI(GLFWwindow* window);

	void processCursorPos(double xpos, double ypos) override;
	void processWheelOffset(double yoffset) override;
	void processMouseBtn(int button, int action) override;
	void processToggleKey(int key, int scancode, int action) override;

	static bool saveCurrentTexture(const std::string& path, int width, int height, uint32_t texture);
};

#endif