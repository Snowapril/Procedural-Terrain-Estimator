#include "TerrainPatch.hpp"

TerrainPatch::TerrainPatch()
	: leftScale(0.0f), topScale(0.0f), rightScale(0.0f), bottomScale(0.0f),
		parent(nullptr), leftAdj(nullptr), topAdj(nullptr), rightAdj(nullptr), bottomAdj(nullptr), originPos(0.0f)
{

}

TerrainPatch::~TerrainPatch()
{
	if (leftAdj)	delete leftAdj;
	if (rightAdj)	delete rightAdj;
	if (topAdj)		delete topAdj;
	if (bottomAdj)	delete bottomAdj;
}

TerrainPatch::TerrainPatch(const TerrainPatch& other)
	: leftScale(other.leftScale), topScale(other.topScale), rightScale(other.rightScale), bottomScale(other.bottomScale), parent(other.parent),
		rightAdj(other.rightAdj), leftAdj(other.leftAdj), topAdj(other.topAdj), bottomAdj(other.bottomAdj), originPos(other.originPos)
{

}

TerrainPatch& TerrainPatch::operator=(const TerrainPatch& other)
{
	if (&other == this)
		return *this;

	leftScale = other.leftScale;
	topScale = other.topScale;
	rightScale = other.rightScale;
	bottomScale = other.bottomScale;
	parent = other.parent;
	rightAdj = other.rightAdj;
	leftAdj = other.leftAdj;
	topAdj = other.topAdj;
	bottomAdj = other.bottomAdj;
	originPos = other.originPos;

	return *this;
}