#ifndef GL_MESH_HPP
#define GL_MESH_HPP

#include <memory>

enum class MeshShape {
	QUAD_PATCH = 0,
	QUAD_TRIANGLES = 1,
};

class GLGeometry;

class GLMesh
{
private:
	std::shared_ptr<GLGeometry> geometrySrc;
public:
	GLMesh() = default;
	~GLMesh() = default;
	GLMesh(const GLMesh& other);
	GLMesh& operator=(const GLMesh& other);
public:
	void drawMesh(unsigned int drawMode) const noexcept;
	bool initWithFixedShape(MeshShape shape);
};

#endif