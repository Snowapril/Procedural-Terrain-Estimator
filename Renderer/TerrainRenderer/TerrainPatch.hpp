/**
* @file		TerrainPatch.hpp
* @author	Shinjihong
* @date		03 September 2018
* @version	1.0.0
* @brief	Quad-tree node class.
* @details	OpenGL Dynamic Terrain Quad-tree node class.
* @see		DynamicTerrain
*/

#ifndef TERRAIN_PATCH_HPP
#define TERRAIN_PATCH_HPP

#include <glm/vec3.hpp>


class TerrainPatch
{
	friend class DynamicTerrain;
private:
	glm::vec3 originPos;
	TerrainPatch *leftTopAdj;
	TerrainPatch *rightTopAdj;
	TerrainPatch *rightBottomAdj;
	TerrainPatch *leftBottomAdj;
	float width;
	float height;
	float scaleNegativeX;
	float scalePositiveX;
	float scaleNegativeZ;
	float scalePositiveZ;
public:
	TerrainPatch() = default;
};

#endif