#ifndef FBM_NOISE_HPP
#define FBM_NOISE_HPP

#include "BrushBoard.hpp"

class FBMNoise : public BrushBoard
{
private:
	float blend;
	float frequency;
	int numOctaves;
public:
	FBMNoise();
	FBMNoise(const Util::Rect& rect);
public:
	void updateGUI(void) override;
	void sendProperties(std::shared_ptr<GLShader> shader) override;
};

#endif