/**
* @file		EngineTerrain.hpp
* @author	Shinjihong
* @date		27 August 2018
* @version	1.0.0
* @brief	this represent terrain itself.
* @details	init with noise map and splat map which will be provided from local (map generator).
			render with tessellation features which are available after opengl version 4.0.
* @see		GLShader
*/

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

	GLShader* terrainShader;
	std::unique_ptr<AssetManager> assetManager;
public:
	EngineTerrain();
	~EngineTerrain();
	EngineTerrain(const EngineTerrain& other);
	EngineTerrain& operator=(const EngineTerrain& other);

public:
	bool initWithLocalFile(float aspectRatio, std::initializer_list<std::string>&& paths);

	void drawTerrain(unsigned int drawMode) const;
	void bakeTerrainMap(int resolutionX, int resolutionY, float aspectRatio);
};

#endif