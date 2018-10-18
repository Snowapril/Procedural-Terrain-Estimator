/**
* @file		DynamicTerrain.hpp
* @author	Shinjihong
* @date		13 September 2018
* @version	1.0.0
* @brief	Dynamic Terrain Tile ( non-uniform patch primitive )
* @details	Compose Dynamic Terrain with quad-tree based approach. if camera position is changed, quad-tree is re-constructed with it's position.
* @see		EngineTerrain
*/

#ifndef DYNAMIC_TERRAIN_HPP
#define DYNAMIC_TERRAIN_HPP

#include <vector>
#include <glm/vec3.hpp>

class TerrainPatch;

class DynamicTerrain
{
private:
	uint32_t VBO;
	uint32_t VAO;

	TerrainPatch* rootPatch;
	TerrainPatch* tailPatch;

	std::vector<float> vertices;
private:
	TerrainPatch * createNode(TerrainPatch* parent, const glm::vec3& originPos, float patchWidth, float patchHeight);
	void divideNode(TerrainPatch* node, const glm::vec3 & cameraPos);
	bool checkDivide(const TerrainPatch* node, glm::vec3 cameraPos);
	void registerToBufferObject(TerrainPatch* patch);
	void calculateTessLevel(TerrainPatch* patch);
	TerrainPatch* findPatch(TerrainPatch* patch, const glm::vec3& targetPos);
	void traverseQuadtree(TerrainPatch* patch);

	void clearTree(void);
	void createTree(const glm::vec3& cameraPos);
protected:
	std::size_t width;
	std::size_t height;

	std::size_t numPatch;

	glm::vec3 terrainCenterPos;
public:
	bool initDynamicTerrain(const glm::vec3& position);
	void updateTerrain(const glm::vec3& cameraPos);
	void drawTerrain(uint32_t drawMode) const;

	glm::vec2 getTerrainScale(void) const;
	void setTerrainScale(std::size_t _width, std::size_t _height);
public:
	DynamicTerrain();
	virtual ~DynamicTerrain();
};

#endif