#ifndef ENGINE_CAMERA_HPP
#define ENGINE_CAMERA_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

enum CameraKey
{
	CAMERA_LEFT		 = 0x01,
	CAMERA_UP		 = 0x02,
	CAMERA_RIGHT	 = 0x04,
	CAMERA_DOWN		 = 0x08,
	CAMERA_LEFT_BTN  = 0x10,
	CAMERA_RIGHT_BTN = 0x20,
};

class EngineCamera
{
public:
	bool updateFov;
	bool toggleZoom;

	float fov;
	float pitch;
	float yaw;
	float speed;

	glm::vec3 position;
	glm::vec3 direction;
	glm::dvec2 prevMousePos;
private:

public:
	EngineCamera();
	EngineCamera(const glm::vec3& pos, const glm::vec3& dir);
	~EngineCamera();
	EngineCamera(const EngineCamera& other);
	EngineCamera& operator=(const EngineCamera& other);
public:
	void processMousePos(double xpos, double ypos) ;
	void processKeyInput(uint32_t keyFlag, float dt) ;
	void processScroll(double yoffset) ;
	void processMouseBtn(uint32_t keyFlag);
	void onUpdate(float dt);
	void sendVP(uint32_t ubo, float aspectRatio) ;
	void flipVertically(float yaxis);

	glm::vec3 getViewPos(void) const ;
};


#endif