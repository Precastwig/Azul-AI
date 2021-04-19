#include "game_elements/Factory.hpp"

void Factory::place(Tile tile) {
	m_tiles.push_back(tile);
}

void Factory::addTiles(std::vector<Tile> tiles) {
	m_tiles.insert(m_tiles.end(), tiles.begin(), tiles.end());
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
