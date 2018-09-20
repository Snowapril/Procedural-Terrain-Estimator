#ifndef DYNAMIC_TERRAIN_HPP
#define DYNAMIC_TERRAIN_HPP

#include <vector>
#include <glm/vec3.hpp>

class TerrainPatch;

class DynamicTerrain
{
private:
	unsigned int VBO;
	unsigned int VAO;

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
protected:
	bool initDynamicTerrain(const glm::vec3& position);
	void updateTerrain(const glm::vec3& cameraPos);
	void drawTerrain(unsigned int drawMode) const;
public:
	DynamicTerrain();
	virtual ~DynamicTerrain();
};

#endif