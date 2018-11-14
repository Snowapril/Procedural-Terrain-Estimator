#ifndef NOISE_GUI_HPP
#define NOISE_GUI_HPP

#include "EventHandler.hpp"
#include <memory>
#include <string>
#include "NonCopyable.hpp"
#include <glm/vec2.hpp>
#include <vector>

class GLShader;
struct GLFWwindow;

class PresetData 
{
	friend class NoiseGUI;
private:
	unsigned int texture;
	std::string name;
public:
	PresetData(unsigned int _texture, std::string _name)
		: texture(_texture), name(_name) {};
};

class NoiseGUI : public NonCopyable
{
protected:
	static bool isInstanciated;
	
	std::vector<PresetData> presetImages;
	float presetBlend;
	unsigned int presetImage;
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
	inline float getPresetBlend(void) const 
	{
		return presetBlend;
	}
	inline unsigned int getPresetImage(void) const
	{
		return presetImage;
	}
};

#endif