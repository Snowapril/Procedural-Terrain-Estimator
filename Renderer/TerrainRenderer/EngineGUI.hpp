#ifndef ENGINE_GUI_HPP
#define ENGINE_GUI_HPP

struct GLFWwindow;

enum class RenderMode : int;
class ImGuiStyle;

#include <vector>

class EngineGUI
{
private:
	bool isGUIOpen;
private:
	static void StyleColorsDark(ImGuiStyle* dst);
public:
	EngineGUI();
	virtual ~EngineGUI();

	bool initGUI(GLFWwindow* window);
	void startUpdateGUI(float dt, float height);
	void endUpdateGUI(float dt);

	void renderGUI(void) const;

	inline bool getIsGUIOpen(void) const
	{
		return isGUIOpen;
	}
};


#endif