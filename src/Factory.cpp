#include "game_elements/Factory.hpp"
#include "Game.hpp"
#include "players/PlayerInfo.hpp"
#include "utils/Sounds.hpp"
#include "utils/helper_enums.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Text.hpp>
#include <atomic>
#include <memory>
#include <string>
#include <utils/Choices.hpp>
#include <cmath>

extern PlayerInfo g_player_info;

Factory::Factory(const int id, const sf::Vector2f position, const float size) : m_id(id), m_size(size), m_tiles(), m_background(), m_hover_minus_points_text(nullptr), m_first_tile(nullptr) {
	m_background.setOrigin(size*2,size*2);
	m_background.setRadius(size * 2);
	m_background.setPosition(position);
	m_background.setFillColor(sf::Color(255,203,208));
};

void Factory::positionTiles() const {
	double angle = 0.0; // Radians
	for (std::shared_ptr<Tile> tile : m_tiles) {
		// Need to do this somewhere
		tile->setOutlineColor(sf::Color::Black); 
		sf::Vector2f newPos = Factory::calculateNewPos(m_background.getPosition(), m_size, angle);
		tile->setPosition(newPos);
		angle += (2 * M_PI / m_tiles.size());
	}
}

void Factory::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	target.draw(m_background, states);
	for (std::shared_ptr<Tile> tile : m_tiles) {
		target.draw(*tile, states);
	}
	if (m_hover_minus_points_text) {
		target.draw(*m_hover_minus_points_text, states);
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
	float distance = hypot((x - pos.x), (y - pos.y));
	//Checks the distance between the mouse and each circle's center
	// double distance = sqrt(pow(pos.x - x, 2) + pow(pos.y - y, 2));
	if (distance <= m_background.getRadius()) {
		g_logger.log(Logger::INFO, "Clicked inside factory");
		return true;
	}
	// g_logger.log(Logger::INFO, distance);
	return false;
}

void Factory::onHover(int x, int y, const TileType& bonus, const bool& isCentre) {
	TileType hovertype = TileType::NONE;
	for (std::shared_ptr<Tile> tile : m_tiles) {
		if (tile->contains(x, y)) {
			if (tile->getOutlineThickness() > 0.0) {
				// Already highlighted
				return;
			}
			if (tile->colour() != bonus) {
				hovertype = tile->colour();
				Sounds::click(1.5);
				break;
			} else if (isOnlyBonus(bonus)) {
				tile->setOutlineThickness(2.0);
				hovertype = tile->colour();
				Sounds::click(1.5);
				break;
			}
		}
	}
	if (!isOnlyBonus(bonus)) {
		bool one_bonus = false;
		for (std::shared_ptr<Tile> tile : m_tiles) {
			if (hovertype == tile->colour()) {
				tile->setOutlineThickness(2.0);
			} else if (hovertype != TileType::NONE && !one_bonus && tile->colour() == bonus) {
				tile->setOutlineThickness(2.0);
				one_bonus = true;
			} else {
				tile->setOutlineThickness(0.0);
			}
		}
	} else {
		//if (hov)
		// Make the hovered bonus tile become unhovered
	}
	std::shared_ptr<Player> curr_player = g_player_info.getCurrentPlayer();
	if (isCentre && !g_player_info.centreTaken()) {
		if (m_first_tile) {
			m_first_tile->setHovered(hovertype != TileType::NONE);
		}
		if (curr_player->points() > 0 && curr_player->getPoisonPoints() > 0) {
			if (hovertype == TileType::NONE) {
				if (m_hover_minus_points_text) {
					// Remove the hover text
					m_hover_minus_points_text = nullptr;
				}
			} else {
				// Hovering over a tile
				if (m_hover_minus_points_text) {
					// Update its position
					m_hover_minus_points_text->setPosition(x + 15, y + 15);
				} else {
					// Create the hover text
					m_hover_minus_points_text = std::make_shared<sf::Text>();
					m_hover_minus_points_text->setFont(g_font);
					m_hover_minus_points_text->setCharacterSize(20);
					m_hover_minus_points_text->setColor(sf::Color::Red);
					m_hover_minus_points_text->setString("-" + std::to_string(curr_player->getPoisonPoints()));
					m_hover_minus_points_text->setPosition(x + 15, y + 15);
				}
			}
		}
	}
}

void Factory::onClick(int x, int y, Game& game) {
	if (!contains(x,y)) {
		return;
	}
	for (std::shared_ptr<Tile> tile : m_tiles) {
		if (tile->contains(x,y)) {
			g_logger.log(Logger::INFO, "Clicked tile " + tile->toString());
			m_hover_minus_points_text = nullptr;
			if (m_first_tile) {
				m_first_tile->setHovered(false);
			}
			if (tile->colour() == game.getBonus()) {
				// We want to allow picking bonus tiles if the factory only contains bonus tiles
				if (isOnlyBonus(game.getBonus())) {
					// Allow it
					PickingChoice choice(TileType::NONE);
					choice.with_bonus = true;
					choice.factory = shared_from_this();
					game.pick_tile(choice);
					Sounds::pop();
					return;
				} else {
					// ??? TODO: something visual
				}
			} else {
				PickingChoice choice(tile->colour());
				choice.with_bonus = hasBonus(game.getBonus());
				choice.factory = shared_from_this();
				game.pick_tile(choice);
				Sounds::pop();
			}
		}
	}
}

std::vector<std::shared_ptr<Tile>> Factory::removeTiles(TileType colour_taken, TileType bonus_type, std::shared_ptr<Factory> centre) {
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
		centre->positionTiles();
	} else {
		// We're the centre! Retain the leftover list
		m_tiles = new_factory_list;
	}
	positionTiles();
	return taken_tiles;
}

bool Factory::isEmpty() const {
	return m_tiles.size() == 0;
}

std::vector<std::shared_ptr<Tile>> Factory::tiles() const {
	return m_tiles;
}

int Factory::numberOf(TileType tile) const {
	int ret = 0;
	for (std::shared_ptr<Tile> t : m_tiles) {
		if (t->colour() == tile) {
			ret++;
		}
	}
	return ret;
}

bool Factory::hasBonus(TileType bonus) const {
	for (std::shared_ptr<Tile> tile : m_tiles) {
		if (tile->colour() == bonus) {
			return true;
		}
	}
	return false;
}

bool Factory::isOnlyBonus(TileType bonus) const {
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
