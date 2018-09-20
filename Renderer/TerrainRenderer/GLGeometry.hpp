#ifndef GL_GEOMETRY_HPP
#define GL_GEOMETRY_HPP

#include <cstdlib>

class GLGeometry
{
private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int IBO;
	std::size_t numElements;
public:
	GLGeometry();
	GLGeometry(unsigned int _vao, unsigned int _vbo, unsigned int _ibo, std::size_t _numElements);
	~GLGeometry();
	GLGeometry(const GLGeometry& other);
	GLGeometry& operator=(const GLGeometry& other);
public:
	inline void setGeometry(unsigned int _vao, unsigned int _vbo, unsigned int _ibo, std::size_t _numElements) noexcept
	{
		VAO			= _vao;
		VBO			= _vbo;
		IBO			= _ibo;
		numElements = _numElements;
	}
	void drawGeometry(unsigned int drawMode) const noexcept;
};

#endif