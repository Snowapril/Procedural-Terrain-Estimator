/**
* @file		GLGeometry.hpp
* @author	Shinjihong
* @date		31 September 2018
* @version	1.0.0
* @brief	OpenGL Geometry Object Wrapper Class
* @details	Wrapping VAO,VBO,IBO to Geometry. by set geometry information(vertex, indices ...) by calling setGeometry method, 
			and render them with drawElements method.
* @see		
*/

#ifndef GL_GEOMETRY_HPP
#define GL_GEOMETRY_HPP

#include <cstdlib>
#include <stdint.h>

class GLGeometry
{
private:
	uint32_t VAO;
	uint32_t VBO;
	uint32_t IBO;
	std::size_t numElements;
public:
	GLGeometry();
	GLGeometry(uint32_t _vao, uint32_t _vbo, uint32_t _ibo, std::size_t _numElements);
	~GLGeometry();
	GLGeometry(const GLGeometry& other);
	GLGeometry& operator=(const GLGeometry& other);
public:
	inline void setGeometry(uint32_t _vao, uint32_t _vbo, uint32_t _ibo, std::size_t _numElements) noexcept
	{
		VAO			= _vao;
		VBO			= _vbo;
		IBO			= _ibo;
		numElements = _numElements;
	}
	void drawGeometry(uint32_t drawMode) const noexcept;
};

#endif