#ifndef VORONOI_NOISE_HPP
#define VORONOI_NOISE_HPP

#include "BrushBoard.hpp"

class VoronoiNoise : public BrushBoard
{
private:
	float blend;
	float frequency;
	float function;
	float distance_type;
	bool multiply_by_F1;
	bool inverse;
public:
	VoronoiNoise();
	VoronoiNoise(const Util::Rect& rect);
public:
	void updateGUI(void) override;
	void sendProperties(std::shared_ptr<GLShader> shader) override;
};

#endif