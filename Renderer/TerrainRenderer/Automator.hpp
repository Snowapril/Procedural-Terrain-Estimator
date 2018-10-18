/**
* @file		Automator.hpp
* @author	Shinjihong
* @date		02 October 2018
* @version	1.0.0
* @brief	return value which is linearly interpolate between elements in vector.
* @details	traverse vector and return value interpolated between current vector element and next vector element.
* @see
*/

#ifndef AUTOMATOR_HPP
#define AUTOMATOR_HPP

#include <vector>
#include "Util.hpp"
#include <algorithm>

template <typename T>
class Automator
{
private:
	std::vector<T> checkPoint;
	float progressBar;
	float duration;
private:
	size_t findEntryPoint(const T& entryPoint)
	{
		size_t minEntry = std::distance(checkPoint.begin(), std::min_element(checkPoint.begin(), checkPoint.end()));

		return minEntry;
	}
public:
	Automator()
		: progressBar(0.0f), duration(0.0f)
	{
	}
public:
	void addAutomation(T&& point)
	{
		checkPoint.emplace_back(std::move(point));
	}

	void setDuration(float _duration)
	{
		duration = _duration;
	}

	void enterLoop(const T& entryPoint)
	{
		size_t closestEntryPoint = findEntryPoint(entryPoint);

		progressBar = static_cast<float>(closestEntryPoint);
	}

	inline T getAutomatedValue(float dt)
	{
		int start = static_cast<int>(progressBar);
		int end = start + 1;

		start %= checkPoint.size();
		end   %= checkPoint.size();

		float fract = Util::fract(progressBar);


		progressBar += dt * checkPoint.size() / duration;
		return Util::interpolate(checkPoint[start], checkPoint[end], fract);
	}
};

#endif