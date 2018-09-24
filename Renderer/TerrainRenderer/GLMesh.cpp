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
	unsigned int VAO, VBO, IBO;
	std::size_t numElements;

	switch (shape)
	{
		case MeshShape::CUBE_PATCH:
		{
			GLfloat cubeVertices[] = {
				-1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
				1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
				1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
				-1.0f, 0.0f,  1.0f, 0.0f, 1.0f,
			};

			unsigned int cubeIndices[] = {
				0, 1, 2, 3
			};

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &IBO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)(sizeof(GLfloat) * 3));

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), &cubeIndices[0], GL_STATIC_DRAW);

			glBindVertexArray(0);

			numElements = sizeof(cubeIndices) / sizeof(unsigned int);
		}
		break;

		case MeshShape::CUBE_TRIANGLES :
		{
			GLfloat cubeVertices[] =
			{
				-1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, -1.0f,
				-1.0f, -1.0f, 1.0f,
				1.0f, -1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				-1.0f, 1.0f, 1.0f,
			};

			unsigned int cubeIndices[] =
			{
				0, 1, 3, 3, 1, 2,
				1, 5, 2, 2, 5, 6,
				5, 4, 6, 6, 4, 7,
				4, 0, 7, 7, 0, 3,
				3, 2, 7, 7, 2, 6,
				4, 5, 0, 0, 5, 1
			};

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &IBO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), &cubeIndices[0], GL_STATIC_DRAW);

			glBindVertexArray(0);

			numElements = sizeof(cubeIndices) / sizeof(unsigned int);
		}
		break;
		case MeshShape::QUAD_TRIANGLE_STRIP :
		{
			GLfloat quadVertices[] = {
				-1.0f, -1.0f, 0.0f, 0.0f,
				-1.0f,  1.0f, 0.0f, 1.0f,
				 1.0f, -1.0f, 1.0f, 0.0f,
				 1.0f,  1.0f, 1.0f, 1.0f,
			};

			unsigned int quadIndices[] =
			{
				0, 1, 2, 3
			};

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &IBO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)(sizeof(GLfloat) * 2));

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), &quadIndices[0], GL_STATIC_DRAW);

			glBindVertexArray(0);

			numElements = sizeof(quadIndices) / sizeof(unsigned int);
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