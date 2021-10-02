#ifndef BOARD
#define BOARD

#include <vector>
#include <memory>
#include "utils/helper_enums.hpp"
#include "utils/cIndex.hpp"

// Pre declarations
class Player;

struct PlacingChoice {
	std::shared_ptr<Location> star;
	Cost cost;
	cIndex index;
	bool operator==(PlacingChoice c) {
		return (star == c.star &&
				index.getIndex() == c.index.getIndex() &&
				cost.colour == c.cost.colour &&
				cost.num_colour == c.cost.num_colour &&
				cost.num_bonus == c.cost.num_bonus);
	};
};

class Board {
public:
	Board();

	void placeTile(PlacingChoice choice, Player* me);
	void keepTiles(std::vector<std::shared_ptr<Tile>> to_keep);

	std::vector<PlacingChoice> getAllPlacingChoices();
	std::vector<Tile::Type> getUnusedColoursInCentre();

	std::string toString(std::shared_ptr<Location> star);
	std::string toString();
private:
	int bonusPiecesAwarded();
	int bonusPointsAwarded();

	// The bonus tiles
	// One window per colour (5 and 6)
	std::vector<bool> m_windows;
	// One statue per colour (where the 1 and 2 of that colour is)
	std::vector<bool> m_statues;
	// One column per colour, (where the 2 and 3 of that colour is)
	std::vector<bool> m_columns;

	// The bonus points
	std::vector<bool> m_full_numbers;

	// The tile spaces
	std::vector<std::shared_ptr<Location>> m_stars;
	std::vector<std::shared_ptr<Tile>> m_keep;
	std::vector<Tile::Type> m_colours_not_in_centre;
};

#endif
