#include "EngineCamera.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "EngineProperty.hpp"
#include "GLShader.hpp"

EngineCamera::EngineCamera()
	: updateFov(false), toggleZoom(false), cameraAutoMode(false), fov((CAMERA_MIN_FOV + CAMERA_MAX_FOV) / 2.0f),
		speed(CAMERA_SPEED), minDepth(CAMERA_MIN_DEPTH), maxDepth(CAMERA_MAX_DEPTH)
{
	initCamera(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

EngineCamera::EngineCamera(const glm::vec3 & pos, const glm::vec3 & dir)
	: updateFov(false), toggleZoom(false), cameraAutoMode(false), fov((CAMERA_MIN_FOV + CAMERA_MAX_FOV) / 2.0f),
		speed(CAMERA_SPEED), minDepth(CAMERA_MIN_DEPTH), maxDepth(CAMERA_MAX_DEPTH)
{
	initCamera(pos, dir);
}

bool EngineCamera::initCamera(const glm::vec3& position, const  glm::vec3& direction)
{
	this->position  = position;
	this->direction = glm::normalize(direction);

	constexpr float EPSILON = 0.001f;

	pitch = glm::degrees(glm::asin(direction.y));
	pitch = Util::clamp(pitch, -89.0f, 89.0f);
	
	yaw = glm::degrees(glm::atan(direction.z / (direction.x + EPSILON)));
	yaw = Util::clamp(yaw, -180.0f, 180.0f);

	posAutomator.setDuration(30.0f);
	posAutomator.addAutomation(glm::vec3(-294.819f, 286.892f, -64.2064f));
	posAutomator.addAutomation(glm::vec3(150.45f, 240.167f, -139.416f));
	posAutomator.addAutomation(glm::vec3(353.321f, 173.672f, 220.698f));
	posAutomator.addAutomation(glm::vec3(488.665f, 281.219f, -277.539f));
	posAutomator.addAutomation(glm::vec3(570.037f, 261.345f, -496.412f));
	posAutomator.addAutomation(glm::vec3(311.954f, 264.521f, -525.097f));
	posAutomator.addAutomation(glm::vec3(-50.6334f, 215.02f, -396.81f));
	posAutomator.addAutomation(glm::vec3(-248.504f, 173.338f, -264.229f));

	dirAutomator.setDuration(30.0f);
	dirAutomator.addAutomation(glm::vec2(234.8f, -23.7f));
	dirAutomator.addAutomation(glm::vec2(119.3f, -21.4f));
	dirAutomator.addAutomation(glm::vec2(-13.999f, -16.6f));
	dirAutomator.addAutomation(glm::vec2(18.6f, -27.9f));
	dirAutomator.addAutomation(glm::vec2(-137.5f, -25.9f));
	dirAutomator.addAutomation(glm::vec2(-59.999f, -32.2f));
	dirAutomator.addAutomation(glm::vec2(-123.4f, -20.9f));
	dirAutomator.addAutomation(glm::vec2(-171.7f, -16.5f));

	return true;
}

void EngineCamera::onUpdate(float dt)
{
	if (cameraAutoMode)
	{
		position = posAutomator.getAutomatedValue(dt);
		
		glm::vec2 dir = dirAutomator.getAutomatedValue(dt);
		yaw = dir.x;
		pitch = dir.y;
	}

	previousVP = project * view;

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

	if (cameraAutoMode)
		return;

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

	pitch = Util::clamp(pitch, -89.0f, 89.0f);

	direction.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
	direction.y = glm::sin(glm::radians(pitch));
	direction.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

	direction = glm::normalize(direction);
}

void EngineCamera::processMouseBtn(uint32_t keyFlag)
{
	if (keyFlag & CAMERA_RIGHT_BTN)
	{
		updateFov  = true;
		toggleZoom = !toggleZoom;
	}
}

void EngineCamera::processKeyInput(uint32_t keyFlag, float dt)
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

void EngineCamera::processKeyCallback(uint32_t keyFlag)
{
	if (keyFlag & CAMERA_AUTO)
		cameraAutoMode = !cameraAutoMode;
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

void EngineCamera::sendVP(uint32_t ubo) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);

	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(project));
	
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void EngineCamera::flipVertically(float yaxis)
{
	float distance = 2 * (position.y - yaxis);
	position.y -= distance;

	pitch = -pitch;

	direction.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
	direction.y = glm::sin(glm::radians(pitch));
	direction.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

	direction = glm::normalize(direction);

	updateView();
}

void EngineCamera::sendVP(const GLShader& shader, bool remove_transition) const
{
	shader.useProgram();

	shader.sendUniform("view", remove_transition ? glm::mat4(glm::mat3(view)) : view);
	shader.sendUniform("project", project);
}

void EngineCamera::updateView(void)
{
	view = glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));
}

void EngineCamera::updateProject(float aspectRatio)
{
	project = glm::perspective(glm::radians(fov), aspectRatio, minDepth, maxDepth);
}

void EngineCamera::setViewportSize(int width, int height)
{
	viewportSize = glm::vec2(width, height);
}