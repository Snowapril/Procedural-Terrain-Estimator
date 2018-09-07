#ifndef GL_GEOMETRY_HPP
#define GL_GEOMETRY_HPP

#include <cstdlib>

class GLGeometry
{
	friend class GLMesh;
private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int IBO;
	std::size_t numElements;
public:
	GLGeometry();
	~GLGeometry();
	GLGeometry(const GLGeometry& other);
	GLGeometry& operator=(const GLGeometry& other);
};

#endif