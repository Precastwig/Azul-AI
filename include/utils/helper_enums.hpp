#ifndef HELPER_ENUMS
#define HELPER_ENUMS

#include <vector>
#include <string>

typedef std::vector<bool> Star;

// Doesn't matter really for now, but if UI is added then it will
enum PlayerColour {
	WHITE,
	WOOD,
	BLACK,
	GREY
};

static const std::vector<PlayerColour> all_player_colours = {
	WHITE,
	WOOD,
	BLACK,
	GREY
};

static const std::vector<std::string> player_colour_strings = {
	"White",
	"Wood",
	"Black",
	"Grey"
};

enum Location {
	ORANGE_STAR,
	RED_STAR,
	BLUE_STAR,
	YELLOW_STAR,
	GREEN_STAR,
	PURPLE_STAR,
	CENTRE_STAR
};

static const std::vector<Location> all_locations = {
	ORANGE_STAR,
	RED_STAR,
	BLUE_STAR,
	YELLOW_STAR,
	GREEN_STAR,
	PURPLE_STAR,
	CENTRE_STAR
};

static const std::vector<std::string> location_strings = {
	"ORANGE STAR",
	"RED STAR",
	"BLUE STAR",
	"YELLOW STAR",
	"GREEN STAR",
	"PURPLE STAR",
	"CENTRE STAR"
};

static const int num_locations = 7;

static const std::vector<int> all_stars_points = {
	17,
	14,
	15,
	16,
	18,
	20,
	12
};

static const std::vector<int> all_nums_points = {
	4, 8, 12, 16
};

enum Tile {
	ORANGE,
	RED,
	BLUE,
	YELLOW,
	GREEN,
	PURPLE,
	NONE
};

static const std::vector<std::string> tile_strings = {
	"Orange",
	"Red",
	"Blue",
	"Yellow",
	"Green",
	"Purple",
	"None"
};

static const std::vector<Tile> all_tiles = {
	ORANGE,
	RED,
	BLUE,
	YELLOW,
	GREEN,
	PURPLE
};

enum Direction {
	UP,
	DOWN
};

#endif
