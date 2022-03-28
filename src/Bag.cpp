#include "game_elements/Bag.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <players/PlayerInfo.hpp>
#include <assert.h>

extern sf::Font g_font;
extern PlayerInfo g_player_info;
extern GameState g_visual_state;

Bag::Bag(sf::Vector2f size, sf::Vector2f position) : m_tile_bin(), m_reward_tiles()	 {
	for (TileType toAdd : Tile::all_tile_types()) {
		// Add 22 of each colour
		for (int i = 0; i < 22; ++i) {
			auto newTile = std::make_shared<Tile>(toAdd);
			m_tile_bag.push_back(newTile);
		}
	}
	// Fill the reward tiles
	double angle = 0.0;
	for (int i = 0; i < 10; ++i) {
		std::shared_ptr<Tile> pulled = pullTile();
		m_reward_tiles.push_back(pulled);
		sf::Vector2f newPos = Factory::calculateNewPos(position, size.x/3.0, angle);
		m_reward_tiles[i]->setPosition(newPos);
		angle += (2 * M_PI / 10);
	}
	m_background.setSize(size);
	sf::FloatRect bgRect = m_background.getLocalBounds();
	m_background.setOrigin(bgRect.left + bgRect.width/2.0f,
                        	bgRect.top  + bgRect.height/2.0f);
	m_background.setPosition(position);
	m_background.setFillColor(sf::Color::Magenta);
	m_txt = sf::Text();
	m_txt.setFont(g_font);
	m_txt.setString("Reward Tiles");
	sf::FloatRect playerRect = m_txt.getLocalBounds();
    m_txt.setOrigin(playerRect.left + playerRect.width/2.0f,
                        playerRect.top  + playerRect.height/2.0f);
	sf::Vector2f textpos = sf::Vector2f(position.x, position.y - size.y + 10);
	m_txt.setPosition(textpos);
}

void Bag::draw (sf::RenderTarget &target, sf::RenderStates states) const {
	target.draw(m_background);
	for (std::shared_ptr<Tile> t : m_reward_tiles) {
		target.draw(*t, states);
	}
	target.draw(m_txt);
}

void Bag::fillFactory(std::shared_ptr<Factory> f) {
	for (int i = 0; i < 4; ++i) {
		std::shared_ptr<Tile> pulled = pullTile();
		assert(pulled != nullptr);
		if (pulled != nullptr) {
			f->place(pulled);
		} else {
			g_logger.log(Logger::WARNING, "Nothing pulled from bag!");
		}
	}
	f->positionTiles();
}

void Bag::toBin(std::vector<std::shared_ptr<Tile>> tiles) {
	// Place a tile in the bin
	m_tile_bin.insert(m_tile_bin.end(), tiles.begin(), tiles.end());
}

void Bag::takeRewardTiles(std::vector<std::shared_ptr<Tile>> tiles) {
	for (std::shared_ptr<Tile> tile : tiles) {
		auto tile_iter = std::find(m_reward_tiles.begin(), m_reward_tiles.end(), tile);
		if (tile_iter != m_reward_tiles.end()) {
			// We find the tile, remove from the reward tiles and replace from bag
			sf::Vector2f tilePos = (*tile_iter)->getPosition();
			m_reward_tiles.erase(tile_iter);
			std::shared_ptr<Tile> replacement = pullTile();
			if (replacement != nullptr) {
				replacement->setPosition(tilePos);
				replacement->setOutlineThickness(0.0);
				m_reward_tiles.push_back(replacement);
			}
		} else {
			// if it's not in there something's gone wrong!
			g_logger.log(Logger::ERROR, "Reward tile selected not found");
		}
	}
}

void Bag::onClick(int x, int y) {
	std::shared_ptr<Player> currPlayer =  g_player_info.getCurrentPlayer();
	int bonus_amount = currPlayer->getBonusToPick();
	for (std::shared_ptr<Tile> tile : m_reward_tiles) {
		if (tile->contains(x, y)) {
			if (bonus_amount > 0) {
				if (m_set_aside_to_take.find(tile) != m_set_aside_to_take.end()) {
					// It already exists
					m_set_aside_to_take.erase(tile);
					bonus_amount++;
					tile->setOutlineThickness(0.0);
					tile->setOutlineColor(sf::Color::Black);
				} else {
					tile->setOutlineColor(sf::Color::White);
					tile->setOutlineThickness(2.0);
					g_logger.log(Logger::INFO, currPlayer->toShortString() + " is taking a " + tile->toString() + " reward tile");
					m_set_aside_to_take.insert(tile);
					bonus_amount--;
				}
				currPlayer->setBonusToPick(bonus_amount);
				break;
			}
		}
	}
	if (bonus_amount == 0) {
		std::vector<std::shared_ptr<Tile>> taken_tiles_vec(m_set_aside_to_take.begin(), m_set_aside_to_take.end());
		for (auto tile_ptr : taken_tiles_vec) {
			tile_ptr->setOutlineColor(sf::Color::Black);
			tile_ptr->setOutlineThickness(0.0);
			currPlayer->addTile(tile_ptr);
		}
		takeRewardTiles(taken_tiles_vec);
		m_set_aside_to_take.clear();
		// Done picking bonus tiles
		g_visual_state.set_placing();
		g_player_info.passOrChangeState();
	}
}

void Bag::binToBag() {
	m_tile_bag.insert(m_tile_bag.end(), m_tile_bin.begin(), m_tile_bin.end());
	m_tile_bin.clear();
}

std::shared_ptr<Tile> Bag::pullTile() {
	if (m_tile_bag.size() == 0) {
		// Refill the bag if empty
		if (m_tile_bin.size() == 0) {
			// Uh Oh
			g_logger.log(Logger::Level::ERROR, "Bag and bin run out of tiles");
			return nullptr;
		}
		g_logger.log(Logger::Level::INFO, "Bag out of tiles");
		binToBag();
		// Make sure binToBag actually filled the tile bag
		assert(!m_tile_bag.empty());
	}
	// Pull a tile
	int tile_index = rand() % m_tile_bag.size();
	std::shared_ptr<Tile> tile_pulled = m_tile_bag[tile_index];
	m_tile_bag.erase(m_tile_bag.begin() + tile_index);
	return tile_pulled;
}
