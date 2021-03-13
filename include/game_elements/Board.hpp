#ifndef BOARD
#define BOARD

#include <vector>
#include "utils/helper_enums.hpp"
#include "utils/cIndex.hpp"

// Pre declarations
class Player;

struct Cost {
	Tile colour;
	int num_colour;
	int num_bonus;
};

struct PlacingChoice {
	Location star;
	Cost cost;
	cIndex index;
};

class Board {
public:
	Board();

	void placeTile(PlacingChoice choice, Player* me);
	void keepTiles(std::vector<Tile> to_keep);

	std::vector<PlacingChoice> getAllPlacingChoices();
private:
	int bonusPiecesAwarded();
	int bonusPointsAwarded();
	void count(Direction dir, Location space, cIndex index, int& score);

	// The bonus tiles
	// One window per colour (5 and 6)
	std::vector<bool> m_windows;
	// One statue per colour (where the 1 and 2 of that colour is)
	std::vector<bool> m_statues;
	// One column per colour, (where the 2 and 3 of that colour is)
	std::vector<bool> m_columns;

	// The bonus points
	std::vector<bool> m_full_stars;
	std::vector<bool> m_full_numbers;

	// The tile spaces
	std::vector<Star> m_tiles;
	std::vector<Tile> m_keep;
};

#endif
