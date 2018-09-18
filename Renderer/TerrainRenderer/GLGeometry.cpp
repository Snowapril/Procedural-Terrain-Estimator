#include "GLGeometry.hpp"
#include <glad/glad.h>

GLGeometry::GLGeometry()
	: VAO(0), VBO(0), IBO(0), numElements(0)
{

}

GLGeometry::GLGeometry(unsigned int _vao, unsigned int _vbo, unsigned int _ibo, std::size_t _numElements)
{
	setGeometry(_vao, _vbo, _ibo, _numElements);
}

GLGeometry::~GLGeometry()
{
	if (VAO)
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &IBO);
	}
}

GLGeometry::GLGeometry(const GLGeometry& other)
	: VAO(other.VAO), VBO(other.VBO), IBO(other.IBO), numElements(other.numElements)
{

}

GLGeometry& GLGeometry::operator=(const GLGeometry& other)
{
	if (&other == this)
		return *this;

	VAO			= other.VAO;
	VBO			= other.VBO;
	IBO			= other.IBO;
	numElements = other.numElements;

	return *this;
}

void GLGeometry::drawGeometry(unsigned int drawMode) const noexcept
{
	glBindVertexArray(VAO);
	glDrawElements(drawMode, numElements, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}