/**
* @file		GLMesh.hpp
* @author	Shinjihong
* @date		03 September 2018
* @version	1.0.0
* @brief	OpenGL Geometry Wrapper (life cycle manage)
* @details	GLMesh class has shared pointer of GLGeometry class. this class manage life cycle of GLGeometry. and also provide method for easy setup.
* @see		GLGeometry
*/

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