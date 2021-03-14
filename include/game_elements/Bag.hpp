#ifndef BAG
#define BAG

#include "utils/helper_enums.hpp"
#include <algorithm>

// This class ecompasses the bag of tiles, the bin, and the reward tile selection
// aka, all locations that tiles originate, and go to die
class Bag {
public:
	Bag() {
		for (Tile toAdd : all_tiles) {
			// Add 22 of each colour
			for (int i = 0; i < 22; ++i) {
				m_tile_bag.push_back(toAdd);
			}
		}
		// Fill the reward tiles
		for (int i = 0; i < 10; ++i) {
			Tile pulled = pullTile();
			m_reward_tiles.push_back(pulled);
		}
	}

	Tile pullTile() {
		if (m_tile_bag.size() == 0) {
			// Refill the bag if empty
			if (m_tile_bin.size() == 0) {
				// Uh Oh
				return NONE;
			}
			binToBag();
		}
		// Pull a tile
		int tile_index = rand() % m_tile_bag.size();
		Tile tile_pulled = m_tile_bag[tile_index];
		m_tile_bag.erase(m_tile_bag.begin() + tile_index);
		return tile_pulled;
	}

	void toBin(std::vector<Tile> tiles) {
		// Place a tile in the bin
		m_tile_bin.insert(m_tile_bin.end(), tiles.begin(), tiles.end());
	}

	void toBin(Tile colour, int number) {
		for (int i = 0; i < number; i++) {
			// Place a tile in the bin
			m_tile_bin.push_back(colour);
		}
	}

	// This function removes the given list of tiles from the
	// reward tiles display
	void takeRewardTiles(std::vector<Tile> tiles) {
		for (Tile tile : tiles) {
			auto tile_iter = std::find(m_reward_tiles.begin(), m_reward_tiles.end(), tile);
			if (tile_iter != m_reward_tiles.end()) {
				// We find the tile, remove from the reward tiles and replace from bag
				m_reward_tiles.erase(tile_iter);
				Tile replacement = pullTile();
				if (replacement != NONE) {
					m_reward_tiles.push_back(replacement);
				}
			} else {
				// if it's not in there something's gone wrong!
				// Error?
			}
		}
	};

	// Gets current selection of reward tiles
	std::vector<Tile> rewardTiles() const {
		return m_reward_tiles;
	};

private:
	void binToBag() {
		m_tile_bag.insert(m_tile_bag.end(), m_tile_bin.begin(), m_tile_bin.end());
		m_tile_bin.clear();
	}

	std::vector<Tile> m_tile_bin;
	std::vector<Tile> m_tile_bag;
	// Reward tiles
	std::vector<Tile> m_reward_tiles;
};

#endif
