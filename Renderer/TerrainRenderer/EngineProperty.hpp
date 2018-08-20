/**
* @file		EngineProperty.hpp
* @author	Shinjihong
* @date		18 July 2018
* @version	1.0.0
* @brief	all Engine properties defined with macro. 
* @see
*/

#ifndef ENGINE_PROPERTY_HPP
#define ENGINE_PROPERTY_HPP

#define CLIENT_WIDTH		1600
#define CLIENT_HEIGHT		900
#define FULL_SCREEN			false
#define ENABLE_4XMSAA		true
#define CAMERA_MIN_FOV		35.0f
#define CAMERA_MAX_FOV		50.0f
#define ENABLE_CULLING		true
#define MOUSE_SENSITIVITY	0.1f
#define CAMERA_SPEED		5.0f

namespace Color {

	const float White[4]           = { 1.0f,  1.0f,  1.0f,  1.0f };
	const float Black[4]           = { 0.0f,  0.0f,  0.0f,  1.0f };
	const float Red[4]             = { 1.0f,  0.0f,  0.0f,  1.0f };
	const float Green[4]           = { 0.0f,  1.0f,  0.0f,  1.0f };
	const float Blue[4]            = { 0.0f,  0.0f,  1.0f,  1.0f };
	const float Yellow[4]          = { 1.0f,  1.0f,  0.0f,  1.0f };
	const float Cyan[4]            = { 0.0f,  1.0f,  1.0f,  1.0f };
	const float Magenta[4]         = { 1.0f,  0.0f,  1.0f,  1.0f };
	const float Silver[4]          = { 0.75f, 0.75f, 0.75f, 1.0f };
	const float LightSteelBlue[4]  = { 0.69f, 0.77f, 0.87f, 1.0f };

}

#endif