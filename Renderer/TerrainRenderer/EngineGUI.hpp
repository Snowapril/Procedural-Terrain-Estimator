/**
* @file		EngineGUI.hpp
* @author	Shinjihong
* @date		04 September 2018
* @version	1.0.0
* @brief	OpenGL GUI Wrapper
* @details	OpenGL Graphics User Interface implemented with imgui open-source api.
* @see		
*/

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