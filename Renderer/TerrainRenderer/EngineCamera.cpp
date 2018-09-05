#include "EngineCamera.hpp"

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

void EngineCamera::sendVP(unsigned int ubo) 
{
}

glm::vec3 EngineCamera::getViewPos(void) const 
{
	return glm::vec3();
}
