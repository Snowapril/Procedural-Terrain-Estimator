#ifndef TERRAIN_PATCH_HPP
#define TERRAIN_PATCH_HPP

class TerrainPatch
{
	friend class EngineTerrain;
private:
	float leftScale;
	float topScale;
	float rightScale;
	float bottomScale;
public:
	TerrainPatch();
	~TerrainPatch();
	TerrainPatch(const TerrainPatch& other);
	TerrainPatch& operator=(const TerrainPatch& other);
};

#endif