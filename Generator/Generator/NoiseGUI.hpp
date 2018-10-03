#ifndef NOISE_GUI_HPP
#define NOISE_GUI_HPP

struct GLFWwindow;

class NoiseGUI
{
protected:
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

protected:
	void updateGUI(float height);
	void rengerGUI(void) const;
public:
	NoiseGUI();
	virtual ~NoiseGUI();
public:
	bool initGUI(GLFWwindow* window);
};

#endif