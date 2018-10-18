/**
* @file		EventHandler.hpp
* @author	Shinjihong
* @date		01 October 2018
* @version	1.0.0
* @brief	OpenGL Callback event handler.
* @details	many Scene objects which need to process callback inputs. 
* @see		
*/

#ifndef EVENT_HANDLER_HPP
#define EVENT_HANDLER_HPP

#include <stdint.h>

class EventHandler
{
public:
	virtual void processKeyInput(uint32_t keyFlag, float dt) = 0;
	virtual void onResizeCallback(int width, int height) = 0;
	virtual void cursorPosCallback(double xpos, double ypos) = 0;
	virtual void scrollCallback(double xoffset, double yoffset) = 0;
	virtual void mouseBtnCallback(uint32_t btnFlag) = 0;
};

#endif