#ifndef HELPER_ENUMS
#define HELPER_ENUMS

#include <vector>
#include <string>
#include <utils/cIndex.hpp>
#include <game_elements/Tile.hpp>

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
	Tile::Type colour;
	int num_colour;
	int num_bonus;
};

class Location {
public:
	// Enum declarations
	enum Direction {
		UP,
		DOWN
	};
	enum Types {
		ORANGE_STAR,
		RED_STAR,
		BLUE_STAR,
		YELLOW_STAR,
		GREEN_STAR,
		PURPLE_STAR,
		CENTRE_STAR
	};
	// Static functions with meta info
	static std::vector<Types> all_locations() {
		return {
			ORANGE_STAR,
			RED_STAR,
			BLUE_STAR,
			YELLOW_STAR,
			GREEN_STAR,
			PURPLE_STAR,
			CENTRE_STAR
		};
	};
	static int num_locations() {
		return all_locations().size();
	}

	// Constructor
	Location(Types l) : m_l(l) {
		for (int j = 0; j < 6; ++j) {
			m_filled.push_back(false);
		}
	};
	// Const inquisitors
	const bool tile(const int& index) {
		return m_filled[index];
	}
	const bool tile(const cIndex& index) {
		return m_filled[index.getIndex()];
	}
	const void place(const int& index) {
		m_filled[index] = true;
	}
	const void place(const cIndex& index) {
		m_filled[index.getIndex()] = true;
	}
	const Types colour() {
		return m_l;
	}
	const std::string toString() {
		return strings[m_l];
	}
	const int pointsForFill() {
		m_scored_fill_points = true;
		return m_points[m_l];
	}
	const bool scoredFillPoints() {
		return m_scored_fill_points;
	}
	const bool operator==(Location l) {
		return m_l == l.m_l;
	}
	const bool operator!=(Location l) {
		return m_l != l.m_l;
	}
	const Cost getCost(const int& index) {
		Cost c;
		c.colour = (Tile::Type)m_l;
		c.num_colour = index + 1;
		c.num_bonus = 0;
		return c;
	}
	const int count(Direction dir, cIndex index) {
		int score;
		if (m_filled[index.getIndex()]) {
			if (score >= 6) {
				return score;
			}
			cIndex next = dir == UP ? index + 1 : index - 1;
			return 1 + count(dir, next);
		}
		return score;
	}
	const bool full() {
		for (bool num : m_filled) {
			if (!num) {
				return false;
			}
		}
		return true;
	}
private:
	std::vector<std::string> strings = {
		"ORANGE STAR",
		"RED STAR",
		"BLUE STAR",
		"YELLOW STAR",
		"GREEN STAR",
		"PURPLE STAR",
		"CENTRE STAR"
	};
	std::vector<int> m_points = {
		17,
		14,
		15,
		16,
		18,
		20,
		12
	};
	Types m_l;
	bool m_scored_fill_points;
	std::vector<bool> m_filled;
};

static const std::vector<int> all_nums_points = {
	4, 8, 12, 16
};

#endif
