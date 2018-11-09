#ifndef SIMPLEX_NOISE_HPP
#define SIMPLEX_NOISE_HPP

#include "BrushBoard.hpp"

class SimplexNoise : public BrushBoard
{
private:
	float blend;
	float frequency;
public:
	SimplexNoise();
	SimplexNoise(const Util::Rect& rect);
public:
	void updateGUI(void) override;
	void sendProperties(std::shared_ptr<GLShader> shader) override;
};

#endif