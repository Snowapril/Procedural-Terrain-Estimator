#include "EngineCamera.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "EngineProperty.hpp"

EngineCamera::EngineCamera()
	: updateFov(false), toggleZoom(false), fov((CAMERA_MIN_FOV + CAMERA_MAX_FOV) / 2.0f),
	pitch(0.0f), yaw(0.0f), speed(CAMERA_SPEED), position(0.0f), direction(0.0f, -1.0f, 0.0f)
{
}

EngineCamera::EngineCamera(const glm::vec3 & pos, const glm::vec3 & dir)
	: updateFov(false), toggleZoom(false), fov((CAMERA_MIN_FOV + CAMERA_MAX_FOV) / 2.0f),
		pitch(0.0f), yaw(0.0f), speed(CAMERA_SPEED), position(pos), direction(dir)
{
}

EngineCamera::~EngineCamera()
{
}

EngineCamera::EngineCamera(const EngineCamera & other)
	: updateFov(other.updateFov), toggleZoom(other.toggleZoom),prevMousePos(other.prevMousePos),
		pitch(other.pitch), yaw(other.yaw), fov(other.fov), speed(other.speed), position(other.position), direction(other.direction)
{

}

EngineCamera & EngineCamera::operator=(const EngineCamera & other)
{
	if (&other == this)
		return *this;

	updateFov	 = other.updateFov;
	toggleZoom	 = other.toggleZoom;
	prevMousePos = other.prevMousePos;
	fov			 = other.fov;
	pitch		 = other.pitch;
	yaw			 = other.yaw;
	speed		 = other.speed;
	position	 = other.position;
	direction	 = other.direction;

	return *this;
}

void EngineCamera::onUpdate(float dt)
{
	if (!updateFov)
		return;

	const float epsilon = 0.1f;

	if (toggleZoom)
	{
		fov -= (fov - CAMERA_MIN_FOV) / (CAMERA_MAX_FOV - CAMERA_MIN_FOV);

		if (fov - epsilon < CAMERA_MIN_FOV) {
			fov = CAMERA_MIN_FOV;
			updateFov = false;
		}
	}
	else
	{
		fov += (CAMERA_MAX_FOV - fov) / (CAMERA_MAX_FOV - CAMERA_MIN_FOV);

		if (fov + epsilon > CAMERA_MAX_FOV) {
			fov = CAMERA_MAX_FOV;
			updateFov = false;
		}
	}
}

void EngineCamera::processMousePos(double xpos, double ypos) 
{
	static bool isFirstUse = true;

	if (isFirstUse)
	{
		isFirstUse = false;
		prevMousePos = glm::dvec2(xpos, ypos);
		return;
	}

	constexpr double sensitivity = MOUSE_SENSITIVITY;

	const double xoffset = (xpos - prevMousePos.x) * sensitivity;
	const double yoffset = (prevMousePos.y - ypos) * sensitivity;

	prevMousePos = glm::dvec2(xpos, ypos);

	yaw		+= xoffset;
	pitch	+= yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	direction.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
	direction.y = glm::sin(glm::radians(pitch));
	direction.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

	direction = glm::normalize(direction);
}

void EngineCamera::processMouseBtn(unsigned int keyFlag)
{
	if (keyFlag & CAMERA_RIGHT_BTN)
	{
		updateFov  = true;
		toggleZoom = !toggleZoom;
	}
}

void EngineCamera::processKeyInput(unsigned int keyFlag, float dt) 
{
	const float movement = speed * dt;
	int pressedKey = 0;

	if (keyFlag & CAMERA_LEFT)
		position -= glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f))) * movement;
	if (keyFlag & CAMERA_RIGHT)
		position += glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f))) * movement;
	if (keyFlag & CAMERA_UP)
		position += movement * direction;
	if (keyFlag & CAMERA_DOWN)
		position -= movement * direction;
}

void EngineCamera::processScroll(double yoffset) 
{
	if (CAMERA_MIN_FOV <= fov && fov <= CAMERA_MAX_FOV)
		fov += yoffset;
	if (fov < CAMERA_MIN_FOV)
		fov = CAMERA_MIN_FOV;
	if (fov > CAMERA_MAX_FOV)
		fov = CAMERA_MAX_FOV;
}

void EngineCamera::sendVP(unsigned int ubo, float aspectRatio)
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	
	glm::mat4 view = glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 project = glm::perspective(glm::radians(fov), aspectRatio, 3.0f, 500.0f);

	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(project));
	
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glm::vec3 EngineCamera::getViewPos(void) const 
{
	return position;
}
