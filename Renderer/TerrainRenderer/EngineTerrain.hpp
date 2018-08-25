#ifndef ENGINE_TERRAIN_HPP
#define ENGINE_TERRAIN_HPP

#include <memory>
#include <initializer_list>
#include <string>

class AssetManager;
class GLShader;

class EngineTerrain
{
private:
	unsigned int terrainMap;

	std::unique_ptr<AssetManager> assetManager;
	GLShader* terrainShader;
public:
	EngineTerrain();
	~EngineTerrain();

public:
	bool initWithLocalFile(float aspectRatio, std::initializer_list<std::string>&& paths);
	bool initWithNoise(int resolutionX, int resolutionY, float aspectRatio);

	void drawTerrain(unsigned int drawMode) const;
	void bakeTerrainMap(int resolutionX, int resolutionY, float aspectRatio);
};

#endif