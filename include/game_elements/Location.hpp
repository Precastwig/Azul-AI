#ifndef LOCATION
#define LOCATION

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Text.hpp>
#include <game_elements/Tile.hpp>
#include <string>
#include <memory>
#include <utils/cIndex.hpp>
#include <utils/helper_enums.hpp>

class Location : sf::Drawable {
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
	static Types location_from_col(Tile::Type col) {
		switch (col) {
			case Tile::ORANGE:
				return ORANGE_STAR;
			case Tile::BLUE:
				return BLUE_STAR;
			case Tile::GREEN:
				return GREEN_STAR;
			case Tile::PURPLE:
				return PURPLE_STAR;
			case Tile::RED:
				return RED_STAR;
			case Tile::YELLOW:
				return YELLOW_STAR;
			default:
				return ORANGE_STAR;
		}
	}
	static Types clockwise_location(Types location) {
		if (location == PURPLE_STAR) {
			return ORANGE_STAR;
		}
		return (Types)(location + 1);
	}

	// Constructor
	Location(Types l) : m_l(l) {
		for (int j = 0; j < 6; ++j) {
			m_filled.push_back(false);
            m_visual_tiles.push_back(std::make_shared<Tile>(get_tile_type()));
            m_tile_nums.push_back(sf::Text());
            m_tile_nums[j].setString(std::to_string(j));
		}
	};

	virtual void draw (sf::RenderTarget &target, sf::RenderStates states) const override {
		for (size_t i = 0; i < m_filled.size(); ++i) {
            target.draw(*m_visual_tiles[i], states);
			target.draw(m_tile_nums[i], states);
        }
	}

	Tile::Type get_tile_type() const {
		switch (m_l) {
			case ORANGE_STAR:
				return Tile::ORANGE;
			case BLUE_STAR:
				return Tile::BLUE;
			case GREEN_STAR:
				return Tile::GREEN;
			case PURPLE_STAR:
				return Tile::PURPLE;
			case RED_STAR:
				return Tile::RED;
			case YELLOW_STAR:
				return Tile::YELLOW;
			default:
				return Tile::ORANGE;
		}
	}
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
	std::vector<std::shared_ptr<Tile>> m_visual_tiles;
    std::vector<sf::Text> m_tile_nums;
};

#endif