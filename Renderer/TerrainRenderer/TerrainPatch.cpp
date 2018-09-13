#include "TerrainPatch.hpp"

TerrainPatch::TerrainPatch()
	:  width(0.0f), height(0.0f), scaleNegativeX(0.0f), scalePositiveX(0.0f), scaleNegativeZ(0.0f), scalePositiveZ(0.0f),
		leftTopAdj(nullptr), rightTopAdj(nullptr), rightBottomAdj(nullptr), leftBottomAdj(nullptr), originPos(0.0f)
{

}

TerrainPatch::~TerrainPatch()
{
	/// no delete adj pointers, because all patches will be managed in EngineTerrain class.
}

TerrainPatch::TerrainPatch(const TerrainPatch& other)
	: width(other.width), height(other.height), scaleNegativeX(other.scaleNegativeX), scalePositiveX(other.scalePositiveX), 
		scaleNegativeZ(other.scaleNegativeZ), scalePositiveZ(other.scalePositiveZ), rightTopAdj(other.rightTopAdj), leftTopAdj(other.leftTopAdj), 
		leftBottomAdj(other.leftBottomAdj), rightBottomAdj(other.rightBottomAdj), originPos(other.originPos)
{

}

TerrainPatch& TerrainPatch::operator=(const TerrainPatch& other)
{
	if (&other == this)
		return *this;

	width			= other.width;
	height			= other.height;
	scaleNegativeX	= other.scaleNegativeX;
	scalePositiveX	= other.scalePositiveX;
	scaleNegativeZ	= other.scaleNegativeZ;
	scalePositiveZ	= other.scalePositiveZ;
	rightTopAdj		= other.rightTopAdj;
	leftTopAdj		= other.leftTopAdj;
	leftBottomAdj	= other.leftBottomAdj;
	rightBottomAdj	= other.rightBottomAdj;
	originPos		= other.originPos;

	return *this;
}