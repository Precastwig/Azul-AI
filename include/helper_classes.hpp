#ifndef HELPER_CLASSES
#define HELPER_CLASSES

#include <vector>
#include <stdlib.h>

typedef std::vector<bool> Star;

// Doesn't matter really for now, but if UI is added then it will
enum PlayerColour {
	WHITE,
	WOOD,
	BLACK,
	GREY
};

enum Location {
	ORANGE_STAR,
	RED_STAR,
	BLUE_STAR,
	YELLOW_STAR,
	GREEN_STAR,
	PURPLE_STAR,
	CENTRE_STAR
};

static const int num_locations = 7;

static const std::vector<int> all_stars_points = {
	17,
	14,
	15,
	16,
	18,
	20,
	12
};

static const std::vector<int> all_nums_points = {
	4, 8, 12, 16
};

enum Tile {
	ORANGE,
	RED,
	BLUE,
	YELLOW,
	GREEN,
	PURPLE,
	NONE
};

static const std::vector<Tile> all_tiles = {
	ORANGE,
	RED,
	BLUE,
	YELLOW,
	GREEN,
	PURPLE
};


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
			if (m_tile_bin.size() == 0) {
				return NONE;
			}
			binToBag();
		}
		int tile_index = rand() % m_tile_bag.size();
		Tile tile_pulled = m_tile_bag[tile_index];
		m_tile_bag.erase(m_tile_bag.begin() + tile_index);
		return tile_pulled;
	}

	void toBin(std::vector<Tile> tiles) {
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

class Factory {
public:
	Factory() : m_centre(nullptr) {};
	Factory(Factory* centre) : m_centre(centre) {};

	void place(Tile tile) {
		m_tiles.push_back(tile);
	}

	void removeTiles(Tile tile_taken, Tile bonus_type) {
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
		if (m_centre) {
			// We're a factory around the edge
			m_tiles.clear();
			// Empty and add to centre
			m_centre->addTiles(new_list);
		} else {
			// We're the centre! Retain the leftover list
			m_tiles = new_list;
		}
	}

	bool isEmpty() {
		return m_tiles.size() == 0;
	}


	std::vector<Tile> tiles() const {
		return m_tiles;
	}

	void addTiles(std::vector<Tile> tiles) {
		m_tiles.insert(m_tiles.end(), tiles.begin(), tiles.end());
	}
private:
	Factory* m_centre;
	std::vector<Tile> m_tiles;
};

class cIndex {
public:
	// max is inclusive
	cIndex() : m_max(6), m_index(1) {};
	cIndex(unsigned int i, unsigned int max) : m_max(max) {
		if (i > m_max) m_index = m_max;
		if (i < 1) m_index = 1;
		m_index = i;
	};

	cIndex operator-(const int& i) const {
		cIndex returnVal(m_index, m_max);
		for (int k = 0; k < i; ++k) {
			returnVal--;
		}
		return returnVal;
	}

	cIndex operator+(const int& i) const {
		cIndex returnVal(m_index, m_max);
		for (int k = 0; k < i; ++k) {
			returnVal++;
		}
		return returnVal;
	}

	cIndex& operator++() {
		m_index == m_max ? m_index = 1 : m_index++;
		return *this;
	}
	cIndex operator++(int) {
		cIndex temp = *this;
		++*this;
		return temp;
	}

	cIndex& operator--() {
		m_index == 1 ? m_index = m_max : m_index--;
		return *this;
	}
	cIndex operator--(int) {
		cIndex temp = *this;
		++*this;
		return temp;
	}

	int getIndex() const { return m_index - 1; };
private:

	unsigned int m_max;
	unsigned int m_index;
};

enum Direction {
	UP,
	DOWN
};

#endif
