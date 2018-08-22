#ifndef ENGINE_TERRAIN_HPP
#define ENGINE_TERRAIN_HPP

#include <memory>

class GLShader;

class EngineTerrain
{
private:
	unsigned int terrainMap;
	std::shared_ptr<GLShader> terrainShader;
public:
	EngineTerrain();
	~EngineTerrain();

public:
	bool init(void);

	void drawTerrain(unsigned int drawMode) const;
	void bakeTerrainMap(int resolutionX, int resolutionY, float aspectRatio);
};


#endif