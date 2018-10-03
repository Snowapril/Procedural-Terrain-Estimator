#ifndef LIGHT_SOURCE_WRAPPER_HPP
#define LIGHT_SOURCE_WRAPPER_HPP

#include <vector>
#include <glm/vec3.hpp>

class GLShader;

struct DirectionLight {
	glm::vec3 position;
	glm::vec3 color;
};

class LightSourceWrapper
{
private:
	std::vector<DirectionLight> dirLights;
public:
	LightSourceWrapper();
	~LightSourceWrapper();
public:
	bool addDirLight(const glm::vec3& pos, const glm::vec3& color);
	void sendLightSources(const GLShader& shader) const;
};

#endif