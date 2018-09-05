#include "EngineCamera.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

EngineCamera::EngineCamera()
{
}

EngineCamera::EngineCamera(const glm::vec3 & pos, const glm::vec3 & dir)
{
}

EngineCamera::~EngineCamera()
{
}

EngineCamera::EngineCamera(const EngineCamera & other)
{
}

EngineCamera & EngineCamera::operator=(const EngineCamera & other)
{
	if (&other == this)
		return *this;

	return *this;
}

void EngineCamera::processMousePos(double xpos, double ypos) 
{
}

void EngineCamera::processMousePos(int btn, int action, int mods)
{

}

void EngineCamera::processKeyInput(void) 
{
}

void EngineCamera::processScroll(double yoffset) 
{
}

void EngineCamera::onResize(float aspectRatio) 
{
}

void EngineCamera::sendVP(unsigned int ubo, float aspectRatio)
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	
	glm::mat4 view = glm::lookAt(glm::vec3(0.f, 100.f, 12.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 project = glm::perspective(glm::radians(90.00f), aspectRatio, 0.1f, 5000.0f);

	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(project));
	
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glm::vec3 EngineCamera::getViewPos(void) const 
{
	return glm::vec3(0.f, 100.f, 12.f);
}
