#ifndef CHOICES
#define CHOICES

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
	std::string to_string() {
		return "Factory " + star->toString() + " " + std::to_string(index.getIndex());
	};
};

#endif