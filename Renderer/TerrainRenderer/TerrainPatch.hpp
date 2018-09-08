#ifndef TERRAIN_PATCH_HPP
#define TERRAIN_PATCH_HPP

#include <glm/vec3.hpp>


class TerrainPatch
{
	friend class EngineTerrain;
private:
	enum class PatchType : int
	{
		ROOT = 0,
		LEFT_TOP = 1,
		LEFT_BOTTOM = 2,
		RIGHT_BOTTOM = 3,
		RIGHT_TOP = 4,
		UNKNOWN = 5
	};

	PatchType type;
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