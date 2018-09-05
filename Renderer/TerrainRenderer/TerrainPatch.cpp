#include "TerrainPatch.hpp"

TerrainPatch::TerrainPatch()
	: type(PatchType::UNKNOWN), patchDepth(0), leftScale(0.0f), topScale(0.0f), rightScale(0.0f), bottomScale(0.0f),
		parent(nullptr), leftTopAdj(nullptr), rightTopAdj(nullptr), rightBottomAdj(nullptr), leftBottomAdj(nullptr), originPos(0.0f)
{

}

TerrainPatch::~TerrainPatch()
{
	/// no delete adj pointers, because all patches will be managed in EngineTerrain class.
}

TerrainPatch::TerrainPatch(const TerrainPatch& other)
	: type(other.type), patchDepth(other.patchDepth), leftScale(other.leftScale), topScale(other.topScale), rightScale(other.rightScale), bottomScale(other.bottomScale), parent(other.parent),
	rightTopAdj(other.rightTopAdj), leftTopAdj(other.leftTopAdj), leftBottomAdj(other.leftBottomAdj), rightBottomAdj(other.rightBottomAdj), originPos(other.originPos)
{

}

TerrainPatch& TerrainPatch::operator=(const TerrainPatch& other)
{
	if (&other == this)
		return *this;

	type			= other.type;
	patchDepth		= other.patchDepth;
	leftScale		= other.leftScale;
	topScale		= other.topScale;
	rightScale		= other.rightScale;
	bottomScale		= other.bottomScale;
	parent			= other.parent;
	rightTopAdj		= other.rightTopAdj;
	leftTopAdj		= other.leftTopAdj;
	leftBottomAdj	= other.leftBottomAdj;
	rightBottomAdj	= other.rightBottomAdj;
	originPos		= other.originPos;

	return *this;
}