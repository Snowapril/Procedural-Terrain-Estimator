#ifndef GL_MESH_HPP
#define GL_MESH_HPP

#include <memory>

enum class MeshShape {
	QUAD_PATCH = 0,
	CUBE_TRIANGLES = 1,
	QUAD_TRIANGLE_STRIP = 2,
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
	void drawMesh(uint32_t drawMode) const noexcept;
	bool initWithFixedShape(MeshShape shape, float scale = 1.0f);
};

#endif