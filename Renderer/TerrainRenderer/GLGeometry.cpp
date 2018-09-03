#include "GLGeometry.hpp"
#include <glad/glad.h>

GLGeometry::GLGeometry()
	: VAO(0), VBO(0), IBO(0), numElements(0)
{

}

GLGeometry::~GLGeometry()
{
	if (VAO != 0)
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

	VAO = other.VAO;
	VBO = other.VBO;
	IBO = other.IBO;
	numElements = other.numElements;

	return *this;
}
