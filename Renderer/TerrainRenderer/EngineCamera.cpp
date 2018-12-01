#include "EngineCamera.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "EngineProperty.hpp"
#include "GLShader.hpp"
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

EngineCamera::EngineCamera()
	: updateFov(false), toggleZoom(false), cameraAutoMode(false), isGrabbed(false), isFirstUse(true), fov((CAMERA_MIN_FOV + CAMERA_MAX_FOV) / 2.0f),
		speed(CAMERA_SPEED), minDepth(CAMERA_MIN_DEPTH), maxDepth(CAMERA_MAX_DEPTH)
{
	initCamera(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1024.0f, 512.0f, 1024.0f));
}

bool EngineCamera::initCamera(const glm::vec3& position, const  glm::vec3& direction, const glm::vec3& terrainScale)
{
	this->position  = position;
	this->direction = glm::normalize(direction);

	constexpr float EPSILON = 0.001f;

	pitch = glm::degrees(glm::asin(direction.y));
	pitch = Util::clamp(pitch, -89.0f, 89.0f);
	
	yaw = glm::degrees(glm::atan(direction.z / (direction.x + EPSILON)));
	yaw = Util::clamp(yaw, -180.0f, 180.0f);

	posAutomator.setDuration(50.0f);
	posAutomator.addAutomation(glm::vec3(-0.14395459f,  0.56033594f, - 0.03135078f) * terrainScale * glm::vec3(2.0f));
	posAutomator.addAutomation(glm::vec3(0.07346191f,  0.46907617f, - 0.06807422f) * terrainScale * glm::vec3(2.0f));
	posAutomator.addAutomation(glm::vec3(0.17252002f, 0.33920312f, 0.1077627f) * terrainScale * glm::vec3(2.0f));
	posAutomator.addAutomation(glm::vec3(0.23860596f,  0.54925586f, - 0.13551709f) * terrainScale * glm::vec3(2.0f));
	posAutomator.addAutomation(glm::vec3(0.27833838f,  0.51043945f, - 0.24238867f) * terrainScale * glm::vec3(2.0f));
	posAutomator.addAutomation(glm::vec3(0.15232129f,  0.51664258f, - 0.25639502f) * terrainScale * glm::vec3(2.0f));
	posAutomator.addAutomation(glm::vec3(-0.02472334f,  0.41996094f, - 0.19375488f) * terrainScale * glm::vec3(2.0f));
	posAutomator.addAutomation(glm::vec3(-0.12133984f,  0.33855078f, - 0.12901807f) * terrainScale * glm::vec3(2.0f));

	dirAutomator.setDuration(50.0f);
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
	if (cameraAutoMode || !isGrabbed)
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
		ImGui::Checkbox("Auto Camera", &cameraAutoMode);
		ImGui::SliderFloat("Speed", &speed, 30.0f, 500.0f, "%.3f");
		ImGui::SliderFloat("Z Near", &minDepth, 1.0f, 50.0f, "%.3f");
		ImGui::SliderFloat("Z Far", &maxDepth, 2000.0f, 10000.0f, "%.3f");
		ImGui::TreePop();
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