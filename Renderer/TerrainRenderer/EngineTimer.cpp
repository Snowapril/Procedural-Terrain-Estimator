#include "EngineTimer.hpp"
#include <Windows.h>

EngineTimer::EngineTimer()
	: deltaTime(0.0), baseTime(0), stopTime(0), pausedTime(0),
		currTime(0), prevTime(0)
{
	__int64 countsPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
	secondsPerCount = 1.0 / countsPerSecond;
}

float EngineTimer::getTotalTime(void) const
{
	if (isStopped)
	{
		return static_cast<float>((stopTime - baseTime - pausedTime) * secondsPerCount);
	}
	else
	{
		return static_cast<float>((currTime - baseTime - pausedTime) * secondsPerCount);
	}
}

float EngineTimer::getDeltaTime(void) const
{
	return static_cast<float>(deltaTime);
}

void EngineTimer::start(void)
{
	__int64 _startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&_startTime);

	if (isStopped)
	{
		pausedTime += _startTime - stopTime;
		prevTime    = _startTime;
		stopTime    = 0;
		isStopped   = false;
	}
}

void EngineTimer::stop(void)
{
	__int64 _stopTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&_stopTime);

	if (!isStopped)
	{
		stopTime  = _stopTime;
		isStopped = true;
	}
}

void EngineTimer::reset(void)
{
	__int64 _currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&_currTime);

	baseTime   = _currTime;
	prevTime   = _currTime;
	currTime   = _currTime;
	pausedTime = 0;
	stopTime   = 0;
	isStopped  = false;
}

void EngineTimer::tick(void)
{
	if (isStopped)
	{
		deltaTime = 0.0;
		return;
	}

	__int64 _currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&_currTime);

	currTime  = _currTime;
	deltaTime = (currTime - prevTime) * secondsPerCount;
	prevTime  = currTime;

	if (deltaTime < 0.0)
		deltaTime = 0.0;
}
