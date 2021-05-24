#ifndef _MATCH3_GAME_DEFS_HPP
#define _MATCH3_GAME_DEFS_HPP
#include <vector>
#include "GemElement.hpp"

using namespace sf;

constexpr unsigned int TOP_BAR_HEIGHT_SIZE_RATIO = 14; //percentage of top bar size to application size 
constexpr unsigned int TOP_BAR_HEIGHT_OFFSET = 100;
constexpr unsigned int GAME_MOVEMENT_SPEED = 11;

constexpr int BOMB_MATCH_TYPE = 100;

using Grid = std::vector< std::vector<GemElement> >;
using MatchPatterns = std::vector< std::vector<std::vector<int> > >;

enum BombTextureIndex: int
{
	HORIZONTAL, VERTICAL, RADIAL
};

enum GameStatus {
	NotStarted,
	Started,
	Won,
	Failed
};

#endif
