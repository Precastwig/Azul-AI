#include "game_elements/Factory.hpp"
#include "Game.hpp"
#include <cmath>

Factory::Factory(const int id, const sf::Vector2f position, const float size) : m_id(id), m_size(size), m_tiles(), m_background() {
	m_background.setOrigin(size*2,size*2);
	m_background.setRadius(size * 2);
	m_background.setPosition(position);
	m_background.setFillColor(Color(255,203,208));
	double angle = 0.0; // Radians
	for (std::shared_ptr<Tile> tile : m_tiles) {
		sf::Vector2f newPos = Factory::calculateNewPos(m_background.getPosition(), m_size, angle);
		// g_logger.log(Logger::INFO, newPos);
		tile->setPosition(newPos);
		// g_logger.log(Logger::INFO, tile.getPosition());
		angle += (2 * M_PI / m_tiles.size());
	}
};

void Factory::draw(RenderTarget &target, RenderStates states) const {
	target.draw(m_background, states);
	for (std::shared_ptr<Tile> tile : m_tiles) {
		// g_logger.log(Logger::INFO, newPos);
		// g_logger.log(Logger::INFO, tile.getPosition());
		target.draw(*tile, states);
	}
}

sf::Vector2f Factory::calculateNewPos(const sf::Vector2f& oldPos, const float& size, const double& angle) {
	return sf::Vector2f(oldPos.x + (size * std::cos(angle)), oldPos.y + (size * std::sin(angle)));
}

void Factory::place(std::shared_ptr<Tile> tile) {
	m_tiles.push_back(tile);
}

void Factory::addTiles(std::vector<std::shared_ptr<Tile>> tiles) {
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
	for (std::shared_ptr<Tile> tile : m_tiles) {
		if (tile->contains(x,y)) {
			g_logger.log(Logger::INFO, "Clicked tile " + tile->toString());
			if (tile->colour() == game.getBonus()) {
				// We want to allow picking bonus tiles if the factory only contains bonus tiles
				if (isOnlyBonus(game.getBonus())) {
					// Allow it
					PickingChoice choice(Tile::Type::NONE);
					choice.with_bonus = true;
					choice.factory = std::shared_ptr<Factory>(this);
					game.pick_tile(choice);
					return;
				} else {
					// ??? TODO: something visual
				}
			} else {
				PickingChoice choice(tile->colour());
				choice.with_bonus = hasBonus(game.getBonus());
				choice.factory = std::shared_ptr<Factory>(this);
				game.pick_tile(choice);
			}
		}
	}
}

std::vector<std::shared_ptr<Tile>> Factory::removeTiles(Tile::Type colour_taken, Tile::Type bonus_type, std::shared_ptr<Factory> centre) {
	std::vector<std::shared_ptr<Tile>> new_factory_list;
	std::vector<std::shared_ptr<Tile>> taken_tiles;
	// We can't take the bonus type of tile, if someone chooses only bonus,
	// then it is represented as choosing NONE, and receiving a bonus
	if (colour_taken == bonus_type) return {};

	bool bonus_taken = false;
	for (std::shared_ptr<Tile> tile : m_tiles) {
		if (tile->colour() != colour_taken) {
			// It's a tile that is not the selected colour
			if (tile->colour() == bonus_type && !bonus_taken) {
				// Remove only one of the bonus tiles
				bonus_taken = true;
				taken_tiles.push_back(tile);
			} else {
				new_factory_list.push_back(tile);
			}
		} else {
			// If we selected this colour, return it
			taken_tiles.push_back(tile);
		}
	}
	
	if (this != centre.get()) {
		// We're a factory around the edge
		m_tiles.clear();
		// Empty and add to centre
		centre->addTiles(new_factory_list);
	} else {
		// We're the centre! Retain the leftover list
		m_tiles = new_factory_list;
	}
	return taken_tiles;
}

bool Factory::isEmpty() const {
	return m_tiles.size() == 0;
}

std::vector<std::shared_ptr<Tile>> Factory::tiles() const {
	return m_tiles;
}

int Factory::numberOf(Tile::Type tile) const {
	int ret = 0;
	for (std::shared_ptr<Tile> t : m_tiles) {
		if (t->colour() == tile) {
			ret++;
		}
	}
	return ret;
}

bool Factory::hasBonus(Tile::Type bonus) const {
	for (std::shared_ptr<Tile> tile : m_tiles) {
		if (tile->colour() == bonus) {
			return true;
		}
	}
	return false;
}

bool Factory::isOnlyBonus(Tile::Type bonus) const {
	for (std::shared_ptr<Tile> tile : m_tiles) {
		if (tile->colour() != bonus) {
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
	for (std::shared_ptr<Tile> t : m_tiles) {
		str += t->toString() + " ";
	}
	return str;
}
