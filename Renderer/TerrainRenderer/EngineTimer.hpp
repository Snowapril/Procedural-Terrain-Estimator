/**
* @file		EngineTimer.hpp
* @author	Shinjihong
* @date		15 July 2018
* @version	1.0.0
* @brief	Timer class for IndieEngine.
* @details  this only support Window platform. not supported for other platform because of QueryPerformanceCounter or QueryPerformanceFrequency.
* @see
*/

#ifndef ENGINE_TIMER_HPP
#define ENGINE_TIMER_HPP

class EngineTimer
{
private:
	double deltaTime;       
	double secondsPerCount;

	__int64 baseTime;
	__int64 currTime;
	__int64 prevTime;
	__int64 pausedTime;
	__int64 stopTime;

	bool isStopped;
public:
	EngineTimer();

	float getTotalTime(void) const;
	float getDeltaTime(void) const;

	/// this can be called when user want to start timer.
	void start(void);  
	/// this can be called when user want to stop timer.
	void stop(void);   
	/// when program is initialized, before entering main loop this method must be called once.
	void reset(void);  
	/// this must be called every tick of IndieEngine.
	void tick(void);   
};


#endif