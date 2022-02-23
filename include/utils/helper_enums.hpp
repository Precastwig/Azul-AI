#ifndef HELPER_ENUMS
#define HELPER_ENUMS

#include <SFML/Graphics/Drawable.hpp>
#include <memory>
#include <vector>
#include <string>
#include <utils/cIndex.hpp>

enum TileType {
	ORANGE,
	RED,
	BLUE,
	YELLOW,
	GREEN,
	PURPLE,
	NONE
};

class PlayerColour {
public:
	enum Colour {
		WHITE,
		WOOD,
		BLACK,
		GREY
	};
	static std::vector<Colour> all_colours() {
		return {
		WHITE,
		WOOD,
		BLACK,
		GREY};
	};
	const std::string toString() {
		return strings[m_c];
	}
	PlayerColour(Colour c) : m_c(c) {};
	const bool operator==(PlayerColour p) {
		return m_c == p.m_c;
	}
	const bool operator!=(PlayerColour p) {
		return m_c != p.m_c;
	}
private:
	Colour m_c;
	std::vector<std::string> strings = {
		"White",
		"Wood",
		"Black",
		"Grey"
	};
};

struct Cost {
	TileType colour;
	int num_colour;
	int num_bonus;
};

static const std::vector<int> all_nums_points = {
	4, 8, 12, 16
};

#endif
