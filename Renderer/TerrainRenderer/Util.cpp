#include "Util.hpp"
#include <glad/glad.h>
#include <vector>
#include <algorithm>

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