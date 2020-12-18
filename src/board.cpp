#include "board.hpp"
#include "player.hpp"

Board::Board()
{
	// Initialise lists
	for (int i = 0; i < num_locations; ++i) {
		std::vector<bool> star;
		for (int j = 0; j < 6; ++j) {
			star.push_back(false);
		}
		m_tiles.push_back(star);
	}

	// Intitialise bonuses
	for (int i = 0; i < 6; ++i) {
		m_windows.push_back(false);
		m_statues.push_back(false);
		m_columns.push_back(false);
		m_full_stars.push_back(false);
	}
	// An extra full star
	m_full_stars.push_back(false);
	for (int i = 0; i < 4; ++i) {
		m_full_numbers.push_back(false);
	}
}

void Board::count(
	Direction dir,
	Location space,
	cIndex index,
	int& score
) {
	if (m_tiles[space][index.getIndex()]) {
		score++;
		if (score >= 6) {
			return;
		}
		cIndex next = dir == UP ? index + 1 : index - 1;
		count(dir, space, next, score);
	}
}

int Board::bonusPiecesAwarded() {
	// There should only be one change every time we call this
	// function, so we can return when we find a true
	for (int i = 1; i <= 6; i++) {
		cIndex star_index(i, 6);
		Star star = m_tiles[star_index.getIndex()];
		if (!m_windows[star_index.getIndex()]) {
			// If the window is covered
			if (star[4] &&
				star[5]) {
				m_windows[star_index.getIndex()] = true;
				return 3;
			}
		}

		if (!m_statues[star_index.getIndex()]) {
			// If the statue is covered
			if (star[1] && star[2]) {
				// We also need to check the "next one"
				Star nextStar = m_tiles[(++star_index).getIndex()];
				if(nextStar[4] && nextStar[3]) {
					m_statues[star_index.getIndex()] = true;
					return 2;
				}
			}
		}
		if (!m_columns[star_index.getIndex()]) {
			// If the column is covered
			if (star[2] && star[3]) {
				// We also need to check if the central pieces are covered
				Star centralStar = m_tiles[CENTRE_STAR];
				if (centralStar[star_index.getIndex()] &&
					centralStar[(star_index - 1).getIndex()]
				) {
					m_columns[star_index.getIndex()] = true;
					return 1;
				}
			}
		}
	}
	return 0;
}

int Board::bonusPointsAwarded() {
	for (int i = 0; i < num_locations; ++i) {
		if (!m_full_stars[i]) {
			Star star = m_tiles[i];
			bool full = true;
			for (unsigned int j = 0; j < star.size(); ++j) {
				if (!star[j]) {
					full = false;
				}
			}
			if (full) {
				m_full_stars[i] = true;
				return all_stars_points[i];
			}
		}
	}
	for (int i = 0; i < 4; ++i) {
		if (!m_full_numbers[i]) {
			bool full = true;
			for (int j = 0; j < num_locations; ++j) {
				Star star = m_tiles[j];
				if (!star[i])
					full = false;
			}
			if (full) {
				m_full_numbers[i] = true;
				return all_nums_points[i];
			}
		}
	}
	return 0;
}

void Board::placeTile(Tile colour, cIndex index, Location space, Player* me) {
	// Place the tile
	m_tiles[space][index.getIndex()] = true;

	// Count points gained
	int points = 0;
	count(UP, space, index, points);
	if (points < 6) {
		// Otherwise we would be double-counting the placed piece
		cIndex next = index - 1;
		count(DOWN, space, next, points);
	}
	points += bonusPointsAwarded();

	// Give the player their points
	me->addPoints(points);

	// Assertain whether we need to allot bonus piece
	int reward = bonusPiecesAwarded();
	if (reward > 0) {
		// and then reward them with some bonus tiles
		me->pickBonusPieces(reward);
	}
}

void Board::keepTiles(std::vector<Tile> to_keep) {
	if (m_keep.size() <= 4)
		m_keep = to_keep;
}
