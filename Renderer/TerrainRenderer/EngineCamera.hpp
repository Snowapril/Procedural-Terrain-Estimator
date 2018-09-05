#ifndef ENGINE_CAMERA_HPP
#define ENGINE_CAMERA_HPP

#include <glm/vec3.hpp>

class EngineCamera
{
private:

private:

public:
	EngineCamera();
	EngineCamera(const glm::vec3& pos, const glm::vec3& dir);
	~EngineCamera();
	EngineCamera(const EngineCamera& other);
	EngineCamera& operator=(const EngineCamera& other);
public:
	void processMousePos(double xpos, double ypos) ;
	void processKeyInput(void) ;
	void processScroll(double yoffset) ;
	void processMousePos(int btn, int action, int mods);
	void onResize(float aspectRatio) ;
	void sendVP(unsigned int ubo) ;
	glm::vec3 getViewPos(void) const ;
};


#endif