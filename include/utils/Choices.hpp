#ifndef CHOICES
#define CHOICES

#include <string>
#include <utils/helper_enums.hpp>
#include <game_elements/Factory.hpp>
#include <game_elements/Location.hpp>

struct PickingChoice {
	PickingChoice(TileType t) : tile_colour(t) {};
	std::shared_ptr<Factory> factory;
	TileType tile_colour;
	bool with_bonus;

	std::string to_string() {
		return "Factory " + std::to_string(factory->id()) + ": " + Tile::toString(tile_colour);
	};
};

struct PlacingChoice {
	std::shared_ptr<Location> star = nullptr;
	Cost cost;
	cIndex index;
	bool operator==(PlacingChoice c) {
		return (star == c.star &&
				index.getIndex() == c.index.getIndex() &&
				cost.colour == c.cost.colour &&
				cost.num_colour == c.cost.num_colour &&
				cost.num_bonus == c.cost.num_bonus);
	};
	int points_gained() const {
		// Count points gained
		int points = 1 + star->count(Location::UP, index + 1);
		if (points < 6) {
			// Otherwise we would be double-counting the placed piece
			points += star->count(Location::DOWN, index - 1);
		}
		return points;
	}
	std::string to_string() const {
		if (star) {
			return star->toString() + " index " + std::to_string(index.getIndex()) + ", " + std::to_string(cost.num_colour) + " " + Tile::toString(cost.colour) + ", " + std::to_string(cost.num_bonus) + " bonus";
		} else {
			return "void choice (nullptr star)";
		}
	};
	static std::vector<PlacingChoice> filterChoicesFromLocation(std::vector<PlacingChoice> choices, LocationType location) {
		std::vector<PlacingChoice> return_list;
		for (PlacingChoice& choice : choices) {
			if (choice.star->colour() == location) {
				return_list.push_back(choice);
			}
		}
		return return_list;
	}
	static std::vector<PlacingChoice> filterChoicesFromIndex(std::vector<PlacingChoice> choices, const int& index) {
		std::vector<PlacingChoice> return_list;
		for (PlacingChoice& choice : choices) {
			if (choice.index.getIndex() == index) {
				return_list.push_back(choice);
			}
		}
		return return_list;
	}
};


#endif