#ifndef COLOURS
#define COLOURS

#include "utils/helper_enums.hpp"
#include <SFML/Graphics/Color.hpp>

using namespace sf;

class Colours {
public:
    static sf::Color tile_to_col(TileType t) {
        switch (t) {
			default:
			case TileType::ORANGE: return (Color(235,149,52));
			break;
			case TileType::RED: return (Color(235,52,52));
			break;
			case TileType::BLUE: return (Color(52,137,235));
			break;
			case TileType::YELLOW: return (Color(235,229,52));
			break;
			case TileType::GREEN: return (Color(70,235,52));
			break;
			case TileType::PURPLE: return (Color(171,52,235));
			break;
			case TileType::NONE: return (Color(0,0,0,0));
		}
    }

	static sf::Color board_tile_unfill_col(TileType t) {
		sf::Color baseCol = tile_to_col(t);
		return sf::Color(
			baseCol.r,
			baseCol.g,
			baseCol.b,
			125
		);
	}

	static sf::Color board_tile_unfill_hover_col(TileType t) {
		sf::Color baseCol = tile_to_col(t);
		return sf::Color(
			baseCol.r,
			baseCol.g,
			baseCol.b,
			200
		);
	}

	static sf::Color button_default_col() {
		return sf::Color(
			255,
			0,
			0,
			125
		); // Red?
	}

	static sf::Color grey() {
		return sf::Color(
			220,
			220,
			220,
			255	
		);
	}

	static sf::Color change_opacity(sf::Color col, const int& op) {
		col.a = op;
		return col;
	}
	
};

#endif