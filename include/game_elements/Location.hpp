#ifndef LOCATION
#define LOCATION

// SFML includes
#include <SFML/Graphics/Drawable.hpp>
#include <game_elements/BoardTile.hpp>
#include <memory>
#include <utils/helper_enums.hpp>
#include "Colours.hpp"

class Game;
class PlacingChoice;

class Location : public std::enable_shared_from_this<Location>, sf::Drawable {
public:
	// Enum declarations
	enum Direction {
		UP,
		DOWN
	};
	// Static functions with meta info
	static std::vector<LocationType> all_locations() {
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
	static std::vector<LocationType> all_locations_except_centre() {
		return {
			ORANGE_STAR,
			RED_STAR,
			BLUE_STAR,
			YELLOW_STAR,
			GREEN_STAR,
			PURPLE_STAR
		};
	}
	static int num_locations() {
		return all_locations().size();
	}
	static LocationType location_from_col(TileType col) {
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
				return CENTRE_STAR;
		}
	}
	static LocationType clockwise_location(LocationType location) {
		if (location == PURPLE_STAR) {
			return ORANGE_STAR;
		}
		return (LocationType)(location + 1);
	}

	// Constructor
	Location(LocationType l);

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

	void onHover(int xpos, int ypos, Game& game);
	void onClick(int x, int y, Game& game);
	void onLeft(TileType bonus);
	void onRight(TileType bonus);

	virtual void draw (sf::RenderTarget &target, sf::RenderStates states) const override;

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
		return m_visual_tiles[index]->isFilled();
	}
	const bool tile(const cIndex& index) {
		return m_visual_tiles[index.getIndex()]->isFilled();
	}
	const void place(const int& index, TileType col) {
		auto tile = m_visual_tiles[index];
		tile->setFilled(true);
		if (m_l == LocationType::CENTRE_STAR) {
			// We need to set the tile to the correct colour
			tile->setTileCol(col);
		}
	}
	const void place(const cIndex& index, TileType col) {
		place(index.getIndex(), col);
	}
	const LocationType colour() {
		return m_l;
	}
	const std::string toString() {
		return strings[m_l];
	}
	const int pointsForFill() {
		return m_points[m_l];
	}
	const void scoredFillPoints() {
		m_scored_fill_points = true;
	}
	const bool isScoredFillPoints() {
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
		int score = 0;
		while(m_visual_tiles[index.getIndex()]->isFilled() && score <= 6) {
			index = dir == UP ? index + 1 : index - 1;
			score++;
		}
		return score;
	}
	const bool full() {
		for (auto t : m_visual_tiles) {
			if (!t->isFilled()) {
				return false;
			}
		}
		return true;
	}
private:
	void updateColours();
	void updateTilesPos();
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
	LocationType m_l;
	bool m_scored_fill_points;
	std::vector<std::shared_ptr<BoardTile>> m_visual_tiles;
	double m_starting_rotation;
	sf::Vector2f m_current_pos;

	// Placing choice selection mechanism
	PlacingChoice get_choice();
	void set_choices(std::vector<PlacingChoice> c, TileType bonus);
    void update_highlight(TileType bonus);
    void wipe_choices(TileType bonus) {
        m_curr_choices.clear();
        update_highlight(bonus);
    }
	cIndex m_index;
    std::vector<PlacingChoice> m_curr_choices; 
};

#endif