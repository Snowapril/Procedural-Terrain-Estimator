#include "GLMesh.hpp"
#include <glad/glad.h>
#include "EngineLogger.hpp"
#include "GLGeometry.hpp"

GLMesh::GLMesh()
{
}

GLMesh::~GLMesh()
{
}

GLMesh::GLMesh(const GLMesh & other)
	: geometrySrc(other.geometrySrc)
{
}

GLMesh & GLMesh::operator=(const GLMesh & other)
{
	if (&other == this)
		return *this;

	geometrySrc = other.geometrySrc;

	return *this;
}



void GLMesh::drawMesh(unsigned int drawMode) const noexcept
{
	glBindVertexArray(geometrySrc->VAO);
	glDrawElements(drawMode, geometrySrc->numElements, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

bool GLMesh::initWithFixedShape(MeshShape shape)
{
	if (geometrySrc) 
		geometrySrc.reset();

	geometrySrc = std::make_shared<GLGeometry>();

	switch (shape)
	{
	case MeshShape::QUAD_PATCH:
	{
		GLfloat quadVertices[] = {
			-1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
			1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
			-1.0f, 0.0f,  1.0f, 0.0f, 1.0f,
		};

		unsigned int quadIndices[] = {
			0, 1, 2, 3
		};

		glGenVertexArrays(1, &geometrySrc->VAO);
		glGenBuffers(1, &geometrySrc->VBO);
		glGenBuffers(1, &geometrySrc->IBO);

		glBindVertexArray(geometrySrc->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, geometrySrc->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)(sizeof(GLfloat) * 3));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometrySrc->IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), &quadIndices[0], GL_STATIC_DRAW);

		glBindVertexArray(0);
	}
		break;
	default:
		return false;
	}

	return true;
}