#ifndef GL_FRAMEBUFFER_HPP
#define GL_FRAMEBUFFER_HPP

//this is framebuffer wrapper class.
class GLFramebuffer
{
private:
	unsigned int FBO;
	unsigned int RBO;
	unsigned int depthTexture;
	unsigned int colorTexture;
public:
	GLFramebuffer();
	~GLFramebuffer();
	GLFramebuffer(const GLFramebuffer& other) = delete;
	GLFramebuffer& operator=(const GLFramebuffer& other) = delete;
public:
	void initFramebuffer(void);
	void attachColorTexture(int width, int height);
	void attachDepthTexture(int width, int height, unsigned int internalformat, unsigned int format);
	void attachDepthbuffer(int width, int height);

	void bindFramebuffer(int width, int height) const;
	void unbindFramebuffer(int width, int height) const;

	bool checkFramebufferStatus(void) const;

	inline unsigned int getColorTexture(void) const {
		return colorTexture;
	}

	inline unsigned int getDepthTexture(void) const {
		return depthTexture;
	}
};


#endif