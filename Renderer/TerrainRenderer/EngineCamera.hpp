#ifndef ENGINE_CAMERA_HPP
#define ENGINE_CAMERA_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

enum CameraKey
{
	CAMERA_LEFT		 = 0x01,
	CAMERA_UP		 = 0x02,
	CAMERA_RIGHT	 = 0x04,
	CAMERA_DOWN		 = 0x08,
	CAMERA_LEFT_BTN  = 0x10,
	CAMERA_RIGHT_BTN = 0x20,
};

class GLShader;

class EngineCamera
{
public:
	bool updateFov;
	bool toggleZoom;

	float fov;
	float pitch;
	float yaw;
	float speed;

	float minDepth;
	float maxDepth;

	glm::vec3 position;
	glm::vec3 direction;
	glm::dvec2 prevMousePos;

	glm::mat4 view;
	glm::mat4 project;
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
	void sendVP(uint32_t ubo) const;
	void sendVP(const GLShader& shader, bool remove_transition = false) const;
	void flipVertically(float yaxis);
	void updateView(void);
	void updateProject(float aspectRatio);

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