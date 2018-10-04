#ifndef ENGINE_CAMERA_HPP
#define ENGINE_CAMERA_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "Automator.hpp"

enum CameraKey
{
	CAMERA_LEFT		 = 0x01,
	CAMERA_UP		 = 0x02,
	CAMERA_RIGHT	 = 0x04,
	CAMERA_DOWN		 = 0x08,
	CAMERA_LEFT_BTN  = 0x10,
	CAMERA_RIGHT_BTN = 0x20,
	CAMERA_AUTO  = 0x40,
};

class GLShader;

class EngineCamera
{
public:
	bool updateFov;
	bool toggleZoom;
	bool cameraAutoMode;

	float fov;
	float pitch;
	float yaw;
	float speed;

	float minDepth;
	float maxDepth;

	glm::vec3 position;
	glm::vec3 direction;

	Automator<glm::vec3> posAutomator;
	Automator<glm::vec2> dirAutomator;

	glm::dvec2 prevMousePos;

	glm::mat4 view;
	glm::mat4 project;
private:
public:
	EngineCamera();
	EngineCamera(const glm::vec3& pos, const glm::vec3& dir);
public:
	void processMousePos(double xpos, double ypos) ;
	void processKeyCallback(uint32_t keyFlag);
	void processKeyInput(uint32_t keyFlag, float dt) ;
	void processScroll(double yoffset) ;
	void processMouseBtn(uint32_t keyFlag);
	void onUpdate(float dt);
	void sendVP(uint32_t ubo) const;
	void sendVP(const GLShader& shader, bool remove_transition = false) const;
	void flipVertically(float yaxis);
	void updateView(void);
	void updateProject(float aspectRatio);

	bool initCamera(const glm::vec3& position, const  glm::vec3& direction);

	inline glm::mat4 getViewMatrix(bool remove_transition = false) const
	{
		return remove_transition ? glm::mat4(glm::mat3(view)) : view;
	}
	inline glm::mat4 getProjectMatrix(void) const
	{
		return project;
	}
	inline glm::vec3 getViewPos(void) const
	{
		return position;
	}
	inline float getMinDepth(void) const
	{
		return minDepth;
	}
	inline float getMaxDepth(void) const
	{
		return maxDepth;
	}
};


#endif