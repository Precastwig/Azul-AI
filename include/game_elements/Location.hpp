#ifndef LOCATION
#define LOCATION

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <game_elements/Tile.hpp>
#include <string>
#include <memory>
#include <utils/cIndex.hpp>
#include <utils/helper_enums.hpp>
#include "Colours.hpp"
#include "game_elements/Factory.hpp"

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
	static Types location_from_col(TileType col) {
		switch (col) {
			case TileType::ORANGE:
				return ORANGE_STAR;
			case TileType::BLUE:
				return BLUE_STAR;
			case TileType::GREEN:
				return GREEN_STAR;
			case TileType::PURPLE:
				return PURPLE_STAR;
			case TileType::RED:
				return RED_STAR;
			case TileType::YELLOW:
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
	Location(Types l) : m_l(l), m_starting_rotation(0.0), m_current_pos() {
		for (size_t j = 0; j < 6; ++j) {
			m_filled.push_back(false);
            m_visual_tiles.push_back(std::make_shared<Tile>(get_tile_type()));
			m_visual_tiles[j]->setOutlineColor(sf::Color::Black);
			m_visual_tiles[j]->setOutlineThickness(2.0);
			m_visual_tiles[j]->setFillColor(get_outline_colour());
            m_tile_nums.push_back(sf::Text());
            m_tile_nums[j].setString(std::to_string(j));
			m_tile_hovered[m_visual_tiles[j]] = false;
		}
		updateTilesPos();
	};

	// Places the star at the centre of the new position
	void setPosition(sf::Vector2f newPos) {
		m_current_pos = newPos;
		updateTilesPos();
	}

	// Sets the rotation to point the 1 towards the given angle
	void setRotation(double rotation) {
		m_starting_rotation = rotation;
		updateTilesPos();
	}

	void onHover(int xpos, int ypos) {
		for (std::shared_ptr<Tile> t : m_visual_tiles) {
			if (t->contains(xpos, ypos)) {
				if (!m_tile_hovered[t]) {
					m_tile_hovered[t] = true;
					t->setFillColor(get_hover_colour());
				}
			} else {
				if (m_tile_hovered[t]) {
					m_tile_hovered[t] = false;
					t->setFillColor(get_outline_colour());
				}
			}
		}
	}

	virtual void draw (sf::RenderTarget &target, sf::RenderStates states) const override {
		for (size_t i = 0; i < m_filled.size(); ++i) {
            target.draw(*m_visual_tiles[i], states);
			target.draw(m_tile_nums[i], states);
        }
	}

	sf::Color get_fill_colour() const {
		return Colours::tile_to_col(get_tile_type());
	}

	sf::Color get_outline_colour() const {
		sf::Color baseCol = get_fill_colour();
		return sf::Color(
			baseCol.r,
			baseCol.b,
			baseCol.g,
			125
		);
	}

	sf::Color get_hover_colour() const {
		sf::Color baseCol = get_fill_colour();
		return sf::Color(
			baseCol.r,
			baseCol.b,
			baseCol.g,
			200
		);
	}

	TileType get_tile_type() const {
		switch (m_l) {
			case ORANGE_STAR:
				return TileType::ORANGE;
			case BLUE_STAR:
				return TileType::BLUE;
			case GREEN_STAR:
				return TileType::GREEN;
			case PURPLE_STAR:
				return TileType::PURPLE;
			case RED_STAR:
				return TileType::RED;
			case YELLOW_STAR:
				return TileType::YELLOW;
			default:
				return TileType::NONE;
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
		c.colour = (TileType)m_l;
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
	void updateTilesPos() {
		double angle = m_starting_rotation;
		for (size_t i = 0; i < m_visual_tiles.size(); ++i) {
			sf::Vector2f tilePos = Factory::calculateNewPos(m_current_pos, 55, angle);
			m_visual_tiles[i]->setPosition(tilePos);
			m_visual_tiles[i]->setRotation(angle * 180 / M_PI);
			angle += (2 * M_PI / m_visual_tiles.size());
		}
	}
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
	std::map<std::shared_ptr<Tile>, bool> m_tile_hovered;
    std::vector<sf::Text> m_tile_nums;
	double m_starting_rotation;
	sf::Vector2f m_current_pos;
};

#endif