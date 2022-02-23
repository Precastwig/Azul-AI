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
};

#endif