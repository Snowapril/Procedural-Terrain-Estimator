#include "GLMesh.hpp"
#include <glad/glad.h>
#include "EngineLogger.hpp"
#include "GLGeometry.hpp"

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
	geometrySrc->drawGeometry(drawMode);
}

bool GLMesh::initWithFixedShape(MeshShape shape)
{
	unsigned int VAO;
	unsigned int VBO;
	unsigned int IBO;
	std::size_t numElements;

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

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &IBO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)(sizeof(GLfloat) * 3));

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), &quadIndices[0], GL_STATIC_DRAW);

			glBindVertexArray(0);

			numElements = 4;
		}
		break;

		case MeshShape::QUAD_TRIANGLES :
		{

		}
		break;

		default:
			return false;
	}

	if (geometrySrc)
		geometrySrc.reset();

	geometrySrc = std::make_shared<GLGeometry>(VAO, VBO, IBO, numElements);

	return true;
}