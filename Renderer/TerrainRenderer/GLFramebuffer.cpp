#include "GLFramebuffer.hpp"
#include "GLDebugger.hpp"
#include <glad/glad.h>

GLFramebuffer::GLFramebuffer()
	: FBO(0), RBO(0), colorTexture(0), depthTexture(0)
{
}

GLFramebuffer::~GLFramebuffer()
{
	if (colorTexture)
		glDeleteTextures(1, &colorTexture);
	if (depthTexture)
		glDeleteTextures(1, &depthTexture);
	if (RBO)
		glDeleteRenderbuffers(1, &RBO);
	if (FBO)
		glDeleteFramebuffers(1, &FBO);
}

void GLFramebuffer::initFramebuffer(void)
{
	glGenFramebuffers(1, &FBO);
}

void GLFramebuffer::attachColorTexture(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLFramebuffer::attachDepthTexture(int width, int height, unsigned int internalformat, unsigned int format)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLFramebuffer::attachDepthbuffer(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLFramebuffer::bindFramebuffer(int width, int height) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLFramebuffer::unbindFramebuffer(int width, int height) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
}

bool GLFramebuffer::checkFramebufferStatus(void) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	CheckError();
	return true;
}