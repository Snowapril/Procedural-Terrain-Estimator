/**
* @file		EngineTimer.hpp
* @author	Shinjihong
* @date		27 August 2018
* @version	1.0.0
* @brief	Timer class for Renderer.
* @details  this only support Window platform. not supported for other platform because of QueryPerformanceCounter or QueryPerformanceFrequency.
* @see
*/

#ifndef ENGINE_TIMER_HPP
#define ENGINE_TIMER_HPP

class EngineTimer
{
private:
	__int64 stopTime;
	__int64 pausedTime;
	__int64 prevTime;
	__int64 currTime;
	__int64 baseTime;
	double deltaTime;
	double secondsPerCount;
	bool isStopped;

public:
	EngineTimer();

	float getTotalTime(void) const;
	inline float getDeltaTime(void) const
	{
		return static_cast<float>(deltaTime);
	}

	/// this can be called when user want to start timer if timer is paused.
	void start(void);  
	/// this can be called when user want to stop timer if timer is running.
	void stop(void);   
	/// when program is initialized, before entering main loop this method must be called once.
	void reset(void);  
	/// this must be called every tick of loop.
	void tick(void);   

	inline bool isTimerPaused(void) const
	{
		return isStopped;
	}
};


#endif