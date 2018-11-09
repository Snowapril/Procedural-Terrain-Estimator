#ifndef BOARD_STORAGE_HPP
#define BOARD_STORAGE_HPP

#include "EngineProperty.hpp"
#include <array>
#include <glm/vec2.hpp>

class BoardStorage
{
private:
	std::array<std::array<unsigned char, BOARD_WIDTH>, BOARD_HEIGHT> board;
	glm::vec2 viewPoint;
public:
	BoardStorage(const std::array<std::array<unsigned char, BOARD_WIDTH>, BOARD_HEIGHT>& _board, const glm::vec2& _viewPoint);
	BoardStorage(BoardStorage&& other);
	BoardStorage& operator=
};

#endif