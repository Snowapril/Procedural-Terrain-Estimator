#ifndef ENGINE_GUI_HPP
#define ENGINE_GUI_HPP

struct GLFWwindow;

enum class RenderMode : int;

#include <vector>

class EngineGUI
{
protected:
	bool isGUIOpen;
public:
	EngineGUI();
	virtual ~EngineGUI();

	bool initGUI(GLFWwindow* window);
	void updateGUI(float dt, float height);
	void renderGUI(void) const;
};


#endif