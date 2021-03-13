#ifndef BAG
#define BAG

#include "utils/helper_enums.hpp"

class Bag {
public:
	Bag() {
		for (Tile toAdd : all_tiles) {
			// Add 22 of each colour
			for (int i = 0; i < 22; ++i) {
				m_tile_bag.push_back(toAdd);
			}
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

private:
	void binToBag() {
		m_tile_bag.insert(m_tile_bag.end(), m_tile_bin.begin(), m_tile_bin.end());
		m_tile_bin.clear();
	}

	std::vector<Tile> m_tile_bin;
	std::vector<Tile> m_tile_bag;
};

#endif
