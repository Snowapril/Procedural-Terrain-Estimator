#include "Util.hpp"
#include <glad/glad.h>
#include <vector>
#include <algorithm>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#ifdef _DEBUG
void Util::ApplyDepthOfField(uint32_t framebuffer, int width, int height, float zNear, float zFar)
{
	std::vector<float> currentFB(width * height * 3);
	std::vector<float> newFB(width * height * 3);
	std::vector<float> DB(width * height);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glReadPixels(0, 0, width, height, GL_RGB, GL_FLOAT, &currentFB[0]);
	glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, &DB[0]);

	auto denormalize = [zNear, zFar](float z) -> float {
		return zNear * zFar / (zFar - z * (zFar - zNear));
	};

	std::for_each(DB.begin(), DB.end(), denormalize);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
#endif

template <> float		Util::clamp			(float value, float begin, float end);
template <> float		Util::fract			(float value);
template <> glm::vec2	Util::fract			(glm::vec2 value);
template <> glm::vec3	Util::fract			(glm::vec3 value);
template <> glm::vec4	Util::fract			(glm::vec4 value);
template <> float		Util::interpolate	(float v1, float v2, float ratio);
template <> glm::vec2	Util::interpolate	(glm::vec2 v1, glm::vec2 v2, float ratio);
template <> glm::vec3	Util::interpolate	(glm::vec3 v1, glm::vec3 v2, float ratio);
template <> glm::vec4	Util::interpolate	(glm::vec4 v1, glm::vec4 v2, float ratio);