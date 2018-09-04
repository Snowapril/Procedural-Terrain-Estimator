#ifndef TERRAIN_PATCH_HPP
#define TERRAIN_PATCH_HPP

#include <glm/vec4.hpp>

class TerrainPatch
{
	friend class EngineTerrain;
private:
	float leftScale;
	float topScale;
	float rightScale;
	float bottomScale;
	TerrainPatch *parent;
	TerrainPatch *leftAdj;
	TerrainPatch *topAdj;
	TerrainPatch *rightAdj;
	TerrainPatch *bottomAdj;
	glm::vec4 originPos;
public:
	TerrainPatch();
	~TerrainPatch();
	TerrainPatch(const TerrainPatch& other);
	TerrainPatch& operator=(const TerrainPatch& other);
};

#endif