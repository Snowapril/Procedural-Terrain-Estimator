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

#include <array>

constexpr int	CLIENT_WIDTH						= 1000;
constexpr int	CLIENT_HEIGHT						= 600;
constexpr bool	FULL_SCREEN							= false;
constexpr bool	ENABLE_4XMSAA						= true;
constexpr bool	ENABLE_CULLING						= false;
constexpr std::array<unsigned int, 9> BRUSH_RADIUS_LIST = { 16U, 32U, 64U, 96U, 128U, 160U, 192U, 224U, 256U };
constexpr uint32_t BRUSH_DEFAULT_RADIUS_IDX			= 2;
constexpr uint32_t  NUM_WORKER						= 8;
constexpr uint32_t	COUNTER_HUDDLE					= 10;
constexpr uint32_t BOARD_WIDTH						= 2048;
constexpr uint32_t BOARD_HEIGHT						= 2048;
constexpr uint32_t BOARD_STORAGE_LIMIT				= 20;

namespace Color {

	constexpr float White[4]			= { 1.0f,  1.0f,  1.0f,  1.0f };
	constexpr float Black[4]			= { 0.0f,  0.0f,  0.0f,  1.0f };
	constexpr float DarkBlue[4]			= { 0.0f,  0.0f, 0.39f,  1.0f };
	constexpr float Red[4]				= { 1.0f,  0.0f,  0.0f,  1.0f };
	constexpr float Green[4]			= { 0.0f,  1.0f,  0.0f,  1.0f };
	constexpr float Blue[4]				= { 0.0f,  0.0f,  1.0f,  1.0f };
	constexpr float Yellow[4]			= { 1.0f,  1.0f,  0.0f,  1.0f };
	constexpr float Cyan[4]				= { 0.0f,  1.0f,  1.0f,  1.0f };
	constexpr float Magenta[4]			= { 1.0f,  0.0f,  1.0f,  1.0f };
	constexpr float Silver[4]			= { 0.75f, 0.75f, 0.75f, 1.0f };
	constexpr float LightSteelBlue[4]	= { 0.69f, 0.77f, 0.87f, 1.0f };
	constexpr float ForestGrren[4]		= { 0.13f, 0.55f, 0.13f, 1.0f };
	constexpr float SteelBlue[4]		= { 0.31f, 0.58f, 0.80f, 1.0f };
	constexpr float DarkViolet[4]		= { 0.58f, 0.00f, 0.83f, 1.0f };

}

#endif