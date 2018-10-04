#ifndef TERRAIN_PATCH_HPP
#define TERRAIN_PATCH_HPP

#include <glm/vec3.hpp>


class TerrainPatch
{
	friend class DynamicTerrain;
private:
	float width;
	float height;
	float scaleNegativeX;
	float scalePositiveX;
	float scaleNegativeZ;
	float scalePositiveZ;
	TerrainPatch *leftTopAdj;
	TerrainPatch *rightTopAdj;
	TerrainPatch *rightBottomAdj;
	TerrainPatch *leftBottomAdj;
	glm::vec3 originPos;
public:
	TerrainPatch() = default;
};

#endif