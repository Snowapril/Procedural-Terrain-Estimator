/**
* @file		EngineProperty.hpp
* @author	Shinjihong
* @date		27 August 2018
* @version	1.0.0
* @brief	all Engine properties defined with macro.
* @see
*/

#ifndef ENGINE_PROPERTY_HPP
#define ENGINE_PROPERTY_HPP

constexpr int	CLIENT_WIDTH		= 1600;
constexpr int	CLIENT_HEIGHT		= 900;
constexpr bool	FULL_SCREEN			= false;
constexpr bool	ENABLE_4XMSAA		= true;
constexpr float CAMERA_MIN_FOV		= 35.0f;
constexpr float CAMERA_MAX_FOV		= 50.0f;
constexpr bool	ENABLE_CULLING		= true;
constexpr float MOUSE_SENSITIVITY	= 0.1f;
constexpr float CAMERA_SPEED		= 30.0f;

namespace Color {

	constexpr float White[4]           = { 1.0f,  1.0f,  1.0f,  1.0f };
	constexpr float Black[4]           = { 0.0f,  0.0f,  0.0f,  1.0f };
	constexpr float DarkBlue[4]        = { 0.0f,  0.0f, 0.39f,  1.0f };
	constexpr float Red[4]             = { 1.0f,  0.0f,  0.0f,  1.0f };
	constexpr float Green[4]           = { 0.0f,  1.0f,  0.0f,  1.0f };
	constexpr float Blue[4]            = { 0.0f,  0.0f,  1.0f,  1.0f };
	constexpr float Yellow[4]          = { 1.0f,  1.0f,  0.0f,  1.0f };
	constexpr float Cyan[4]            = { 0.0f,  1.0f,  1.0f,  1.0f };
	constexpr float Magenta[4]         = { 1.0f,  0.0f,  1.0f,  1.0f };
	constexpr float Silver[4]          = { 0.75f, 0.75f, 0.75f, 1.0f };
	constexpr float LightSteelBlue[4]  = { 0.69f, 0.77f, 0.87f, 1.0f };
	constexpr float ForestGrren[4]     = { 0.13f, 0.55f, 0.13f, 1.0f };
	constexpr float SteelBlue[4]       = { 0.31f, 0.58f, 0.80f, 1.0f };
	constexpr float DarkViolet[4]      = { 0.58f, 0.00f, 0.83f, 1.0f };

}

#endif