#ifndef NOISE_GUI_HPP
#define NOISE_GUI_HPP

#include <memory>
class GLShader;

struct GLFWwindow;

class NoiseGUI
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
	void updateGUI(float height);
	void sendProperties(std::shared_ptr<GLShader> shader);
	void renderGUI(void) const;
public:
	NoiseGUI();
	NoiseGUI(const NoiseGUI& other) = delete;
	NoiseGUI& operator=(const NoiseGUI& other) = delete;
	~NoiseGUI();
public:
	bool initGUI(GLFWwindow* window);
};

#endif