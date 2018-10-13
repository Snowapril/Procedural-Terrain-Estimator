/**
* @file		GLFramebuffer.hpp
* @author	Shinjihong
* @date		20 September 2018
* @version	1.0.0
* @brief	OpenGL Framebuffer Object Wrapper class.
* @details	manage lify cycle of FBO and it's components(RBO, color attachment, depth attachment). provide method for bind and unbind framebuffer.
			and also provide getter method for color attachment and depth attachment.
*/

#ifndef GL_FRAMEBUFFER_HPP
#define GL_FRAMEBUFFER_HPP

#include <stdint.h>

//this is framebuffer wrapper class.
class GLFramebuffer
{
private:
	uint32_t FBO;
	uint32_t RBO;
	uint32_t depthTexture;
	uint32_t colorTexture;
public:
	GLFramebuffer();
	~GLFramebuffer();
	GLFramebuffer(const GLFramebuffer& other) = delete;
	GLFramebuffer& operator=(const GLFramebuffer& other) = delete;
public:
	void initFramebuffer(void);
	void attachColorTexture(int width, int height, uint32_t wrapMethod, bool hdr);
	void attachDepthTexture(int width, int height, uint32_t internalformat, uint32_t format);
	void attachDepthbuffer(int width, int height);

	void bindFramebuffer(int width, int height) const;
	void unbindFramebuffer(int width, int height) const;

	bool configureFramebuffer(void) const;

	inline uint32_t getColorTexture(void) const {
		return colorTexture;
	}

	inline uint32_t getDepthTexture(void) const {
		return depthTexture;
	}
};


#endif
