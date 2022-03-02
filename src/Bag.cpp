#include "game_elements/Bag.hpp"
#include <assert.h>

extern sf::Font g_font;

Bag::Bag(sf::Vector2f size, sf::Vector2f position) {
	for (TileType toAdd : Tile::all_tile_types()) {
		// Add 22 of each colour
		for (int i = 0; i < 22; ++i) {
			auto newTile = std::make_shared<Tile>(toAdd);
			m_tile_bag.push_back(newTile);
		}
	}
	// Fill the reward tiles
	for (int i = 0; i < 10; ++i) {
		std::shared_ptr<Tile> pulled = pullTile();
		m_reward_tiles.push_back(pulled);
	}
	m_background.setSize(size);
	m_background.setPosition(position);
	m_txt = sf::Text();
	m_txt.setFont(g_font);
	sf::FloatRect playerRect = m_txt.getLocalBounds();
    m_txt.setOrigin(playerRect.left + playerRect.width/2.0f,
                        playerRect.top  + playerRect.height/2.0f);
	m_txt.setString("Reward Tiles");
}

void Bag::draw (RenderTarget &target, RenderStates states) const {
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
			m_reward_tiles.erase(tile_iter);
			std::shared_ptr<Tile> replacement = pullTile();
			if (replacement != nullptr) {
				m_reward_tiles.push_back(replacement);
			}
		} else {
			// if it's not in there something's gone wrong!
			g_logger.log(Logger::ERROR, "Reward tile selected not found");
		}
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
