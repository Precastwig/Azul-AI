#include "game_elements/Factory.hpp"
#include "Game.hpp"
#include <cmath>

void Factory::draw(RenderTarget &target, RenderStates states) const {
	target.draw(m_background, states);
	double angle = 0.0; // Radians
	for (Tile tile : m_tiles) {
		sf::Vector2f newPos = Factory::calculateNewPos(m_background.getPosition(), m_size, angle);
		tile.setPosition(newPos);
		target.draw(tile, states);
		angle += (2 * M_PI / m_tiles.size());
	}
}

sf::Vector2f Factory::calculateNewPos(const sf::Vector2f& oldPos, const float& size, const double& angle) {
	return sf::Vector2f(oldPos.x + (size * std::cos(angle)), oldPos.y + (size * std::sin(angle)));
}

void Factory::place(Tile tile) {
	m_tiles.push_back(tile);
}

void Factory::addTiles(std::vector<Tile> tiles) {
	m_tiles.insert(m_tiles.end(), tiles.begin(), tiles.end());
}

bool Factory::contains(int x, int y) {
	sf::Vector2f pos = m_background.getPosition();
	float distance = hypot((x - pos.x), (y - pos.y)); //Checks the distance between the mouse and each circle's center
	// double distance = sqrt(pow(pos.x - x, 2) + pow(pos.y - y, 2));
	if (distance <= m_background.getRadius()) {
		g_logger.log(Logger::INFO, "Clicked inside factory");
		// g_logger.log(Logger::INFO, "Factory: " + std::to_string(pos.x) + ", " + std::to_string(pos.y));
		// g_logger.log(Logger::INFO, "Radius: " + std::to_string( m_background.getRadius()));
		// g_logger.log(Logger::INFO, "Click: " + std::to_string(x) + ", " + std::to_string(y));

		return true;
	}
	// g_logger.log(Logger::INFO, distance);
	return false;
}

void Factory::onClick(int x, int y, Game& game) {
	for (Tile tile : m_tiles) {
		if (tile.contains(x,y)) {
			g_logger.log(Logger::INFO, "Clicked tile " + tile.toString());
			if (tile == game.getBonus()) {
				// We want to allow picking bonus tiles if the factory only contains bonus tiles
				if (isOnlyBonus(game.getBonus())) {
					// Allow it
					PickingChoice choice(Tile::NONE);
					choice.with_bonus = true;
					choice.factory = std::shared_ptr<Factory>(this);
					game.pick_tile(choice);
					return;
				} else {
					// ??? TODO: something visual
				}
			} else {
				PickingChoice choice(tile);
				choice.with_bonus = hasBonus(game.getBonus());
				choice.factory = std::shared_ptr<Factory>(this);
				game.pick_tile(choice);
			}
		}
	}
}

void Factory::removeTiles(Tile tile_taken, Tile bonus_type, std::shared_ptr<Factory> centre) {
	std::vector<Tile> new_list;
	// We can't take the bonus type of tile
	if (tile_taken == bonus_type) return;

	bool bonus_taken = false;
	for (Tile tile : m_tiles) {
		if (tile != tile_taken) {
			if (tile == bonus_type && !bonus_taken) {
				// Remove only one of the bonus tiles
				bonus_taken = true;
			} else {
				new_list.push_back(tile);
			}
		}
	}
	if (this != centre.get()) {
		// We're a factory around the edge
		m_tiles.clear();
		// Empty and add to centre
		centre->addTiles(new_list);
	} else {
		// We're the centre! Retain the leftover list
		m_tiles = new_list;
	}
}

bool Factory::isEmpty() const {
	return m_tiles.size() == 0;
}

std::vector<Tile> Factory::tiles() const {
	return m_tiles;
}

int Factory::numberOf(Tile tile) const {
	int ret = 0;
	for (Tile t : m_tiles) {
		if (t == tile) {
			ret++;
		}
	}
	return ret;
}

bool Factory::hasBonus(Tile bonus) const {
	for (Tile tile : m_tiles) {
		if (tile == bonus) {
			return true;
		}
	}
	return false;
}

bool Factory::isOnlyBonus(Tile bonus) const {
	for (Tile tile : m_tiles) {
		if (tile != bonus) {
			return false;
		}
	}
	return true;
}


std::string Factory::toString() {
	std::string str = "";
	if (m_tiles.size() == 0) {
		return "Empty";
	}
	for (Tile t : m_tiles) {
		str += t.toString() + " ";
	}
	return str;
}
