#include "EngineCamera.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "EngineProperty.hpp"
#include "GLShader.hpp"
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

EngineCamera::EngineCamera()
	: updateFov(false), toggleZoom(false), isGrabbed(false), isFirstUse(true), isCameraFixed(false), fov((CAMERA_MIN_FOV + CAMERA_MAX_FOV) / 2.0f),
		speed(CAMERA_SPEED), minDepth(CAMERA_MIN_DEPTH), maxDepth(CAMERA_MAX_DEPTH)
{
	initCamera(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1024.0f, 512.0f, 1024.0f));
}

enum CameraView {
	TOP_VIEW = 0,
	LEFT_TO_MIDDLE = 1,
	RIGHT_TO_MIDDLE = 2,
	FRONT_TO_MIDDLE = 3,
	BACK_TO_MIDDLE = 4
};

bool EngineCamera::initCamera(const glm::vec3& position, const  glm::vec3& direction, const glm::vec3& terrainScale)
{
	this->position  = position;
	this->direction = glm::normalize(direction);

	std::tie(pitch, yaw) = directionToEulerDegrees(direction);

	initCameraView(terrainScale);

	return true;
}

void EngineCamera::initCameraView(const glm::vec3& terrainScale) 
{
	fixedPosition[TOP_VIEW] = glm::vec3(0.0f, min(terrainScale.x, terrainScale.z) * (glm::radians(fov) + 0.6f), 0.0f);
	fixedDirection[TOP_VIEW] = glm::vec3(0.0f, -1.0f, 0.0f);

	fixedPosition[LEFT_TO_MIDDLE] = glm::vec3(-terrainScale.x, terrainScale.y, 0.0f);
	fixedDirection[LEFT_TO_MIDDLE] = glm::vec3(1.0f, 0.0f, 0.0f);

	fixedPosition[RIGHT_TO_MIDDLE] = glm::vec3(terrainScale.x, terrainScale.y, 0.0f);
	fixedDirection[RIGHT_TO_MIDDLE] = glm::vec3(-1.0f, 0.0f, 0.0f);

	fixedPosition[FRONT_TO_MIDDLE] = glm::vec3(0.0f, terrainScale.y, -terrainScale.z);
	fixedDirection[FRONT_TO_MIDDLE] = glm::vec3(0.0f, 0.0f, 1.0f);

	fixedPosition[BACK_TO_MIDDLE] = glm::vec3(0.0f, terrainScale.y, terrainScale.z);
	fixedDirection[BACK_TO_MIDDLE] = glm::vec3(0.0f, 0.0f, -1.0f);
}

void EngineCamera::onUpdate(float dt)
{
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
	if (!isGrabbed)
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
	if (keyFlag & CAMERA_LEFT_BTN)
	{
		isFirstUse = true;
		isGrabbed = true;
	}
	else
		isGrabbed = false;

	if (keyFlag & CAMERA_RIGHT_BTN)
	{
		updateFov  = true;
		toggleZoom = !toggleZoom;
	}
}

void EngineCamera::updateGUI(void)
{
	if (ImGui::TreeNode("Camera Settings"))
	{
		ImGui::SliderFloat("Speed", &speed, 30.0f, 500.0f, "%.3f");
		ImGui::SliderFloat("Z Near", &minDepth, 1.0f, 50.0f, "%.3f");
		ImGui::SliderFloat("Z Far", &maxDepth, 2000.0f, 30000.0f, "%.3f");
		ImGui::TreePop();
	}
}

void EngineCamera::processKeyInput(uint32_t keyFlag, float dt)
{
	const float movement = speed * dt;
	int pressedKey = 0;

	
	if (!isCameraFixed) {
		if (keyFlag & CAMERA_LEFT) {
			position -= glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f))) * movement;
		}
		if (keyFlag & CAMERA_RIGHT) {
			position += glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f))) * movement;
		}
		if (keyFlag & CAMERA_UP) {
			position += movement * direction;
		}
		if (keyFlag & CAMERA_DOWN) {
			position -= movement * direction;
		}
	}
}

void EngineCamera::processKeyCallback(uint32_t keyFlag)
{
	isCameraFixed = false;
	if (keyFlag & CAMERA_1) {
		position = fixedPosition[0];
		direction = fixedDirection[0];
		std::tie(pitch, yaw) = directionToEulerDegrees(direction);
		isCameraFixed = true;
	}
	else if (keyFlag & CAMERA_2) {
		position = fixedPosition[1];
		direction = fixedDirection[1];
		std::tie(pitch, yaw) = directionToEulerDegrees(direction);
		isCameraFixed = true;
	}
	else if (keyFlag & CAMERA_3) {
		position = fixedPosition[2];
		direction = fixedDirection[2];
		std::tie(pitch, yaw) = directionToEulerDegrees(direction);
		isCameraFixed = true;
	}
	else if (keyFlag & CAMERA_4) {
		position = fixedPosition[3];
		direction = fixedDirection[3];
		isCameraFixed = true;
		std::tie(pitch, yaw) = directionToEulerDegrees(direction);
	}
	else if (keyFlag & CAMERA_5) {
		position = fixedPosition[4];
		direction = fixedDirection[4];
		std::tie(pitch, yaw) = directionToEulerDegrees(direction);
		isCameraFixed = true;
	}
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

std::tuple<float, float> EngineCamera::directionToEulerDegrees(const glm::vec3& direction)
{
	constexpr float EPSILON = 0.001f;
	float pitch, yaw;

	pitch = glm::degrees(glm::asin(direction.y));
	pitch = Util::clamp(pitch, -89.0f, 89.0f);

	if (direction == glm::vec3(-1.0f, 0.0f, 0.0f)) { // direction (1, 0, 0) and (-1, 0, 0) are converted to 0 and -0. 
		yaw = 180.0f;
	}
	else {
		yaw = glm::degrees(glm::atan(direction.z / (direction.x + EPSILON)));
	}


	return std::make_tuple(pitch, yaw);
}