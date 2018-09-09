#ifndef TERRAIN_PATCH_HPP
#define TERRAIN_PATCH_HPP

#include <glm/vec3.hpp>


class TerrainPatch
{
	friend class EngineTerrain;
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
	TerrainPatch();
	~TerrainPatch();
	TerrainPatch(const TerrainPatch& other);
	TerrainPatch& operator=(const TerrainPatch& other);
};

#endif