#include "game_elements/Board.hpp"
#include "players/Player.hpp"
#include <algorithm>
#include <iterator>
#include <vector>

Board::Board()
{
	// Initialise lists
	for (Location::Types type : Location::all_locations()) {
		auto star = std::make_shared<Location>(type);
		m_stars.push_back(star);
	}

	// Intitialise bonuses
	for (int i = 0; i < 6; ++i) {
		m_windows.push_back(false);
		m_statues.push_back(false);
		m_columns.push_back(false);
	}
	// An extra full star
	for (int i = 0; i < 4; ++i) {
		m_full_numbers.push_back(false);
	}
	m_colours_not_in_centre = Tile::all_tile_types();
}

void Board::draw (sf::RenderTarget &target, sf::RenderStates states) const {
	// Oh god.
	// Draw each "star" of placing choices, they're not really placing choices
	for (std::shared_ptr<Location> loc : m_stars) {
		loc->draw(target, states);
	}
	
	// Draw windows/statues/columns


	// on hover, show all placing choices for that space,
	// Then on click, bring that submenu up permanently
	// Then allow user to click on each of them
}

std::vector<Tile::Type> Board::getAdjacentStarColours(Tile::Type starCol) {
	std::vector<Tile::Type> returnList;
	if (starCol == Tile::ORANGE) {
		returnList.push_back(Tile::PURPLE);
	} else {
		returnList.push_back((Tile::Type)(starCol-1));
	}

	if (starCol == Tile::PURPLE) {
		returnList.push_back(Tile::ORANGE);
	} else {
		returnList.push_back((Tile::Type)(starCol+1));
	}
	return returnList;
}

int Board::bonusPiecesAwarded() {
	// There should only be one change every time we call this
	// function, so we can return when we find a true
	for (int i = 1; i <= 6; i++) {
		cIndex star_index(i, 6);
		std::shared_ptr<Location> star = m_stars[star_index.getIndex()];
		if (!m_windows[star_index.getIndex()]) {
			// If the window is covered
			if (star->tile(4) &&
				star->tile(5)) {
				m_windows[star_index.getIndex()] = true;
				return 3;
			}
		}

		if (!m_statues[star_index.getIndex()]) {
			// If the statue is covered
			if (star->tile(1) && star->tile(2)) {
				// We also need to check the "next one"
				std::shared_ptr<Location> nextStar = m_stars[(++star_index).getIndex()];
				if(nextStar->tile(4) && nextStar->tile(3)) {
					m_statues[star_index.getIndex()] = true;
					return 2;
				}
			}
		}
		if (!m_columns[star_index.getIndex()]) {
			// If the column is covered
			if (star->tile(2) && star->tile(3)) {
				// We also need to check if the central pieces are covered
				std::shared_ptr<Location> centralStar = m_stars[Location::CENTRE_STAR];
				if (centralStar->tile(star_index) &&
					centralStar->tile(star_index - 1)
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
	int points = 0;
	for (std::shared_ptr<Location> star : m_stars) {
		if (star->full()) {
			points += star->pointsForFill();
		}
	}
	for (int i = 0; i < 4; ++i) {
		if (!m_full_numbers[i]) {
			bool full = true;
			for (std::shared_ptr<Location> star : m_stars) {
				if (!star->tile(i)) {
					full = false;
				}
			}
			if (full) {
				m_full_numbers[i] = true;
				points += all_nums_points[i];
			}
		}
	}
	return points;
}

std::vector<Tile::Type> Board::getUnusedColoursInCentre() {
	return m_colours_not_in_centre;
}

void Board::placeTile(PlacingChoice choice, Player* me) {
	// Place the tile
	choice.star->place(choice.index);

	if (choice.star->colour() == Location::CENTRE_STAR) {
		// We need to log what colour we're placing
		m_colours_not_in_centre.erase(std::remove(m_colours_not_in_centre.begin(), m_colours_not_in_centre.end(), choice.cost.colour));
	}

	// Count points gained
	int points = choice.star->count(Location::UP, choice.index);
	if (points < 6) {
		// Otherwise we would be double-counting the placed piece
		cIndex next = choice.index - 1;
		points += choice.star->count(Location::DOWN, next);
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

std::vector<PlacingChoice> Board::getPlacingChoicesOfCol(Tile::Type col) {
	std::vector<PlacingChoice> allChoices = getAllPlacingChoices();
	std::vector<PlacingChoice> returnList;
	std::copy_if(allChoices.begin(), allChoices.end(), std::back_inserter(returnList), 
		[col](PlacingChoice choice){
			return choice.cost.colour == col;
		});
	return returnList;
}

std::vector<PlacingChoice> Board::getAllPlacingChoices() {
	std::vector<PlacingChoice> choices;
	for (std::shared_ptr<Location> star : m_stars) {
		for (unsigned int i = 0; i < 6; ++i) {
			bool filled = star->tile(i);
			if (!filled) {
				PlacingChoice choice;
				choice.star = star;
				choice.cost = star->getCost(i);
				choice.index = cIndex(i + 1, 6);
				choices.push_back(choice);
			}

		}
	}
	return choices;
}

int Board::tilesNeededToGetStatue(Location::Types star) {
	// Check 3/4 of tilcol
	int tilesNeededForColumn = (m_stars[star]->tile(1)) ? 1 : 0;
	tilesNeededForColumn += (m_stars[star]->tile(2)) ? 2 : 0;
	if (tilesNeededForColumn == 0) {
		return 0;
	}
	Location::Types nextStar = Location::clockwise_location(star);
	tilesNeededForColumn += (m_stars[nextStar]->tile(3)) ? 3 : 0;
	tilesNeededForColumn += (m_stars[nextStar]->tile(4)) ? 4 : 0;
	return tilesNeededForColumn;
}

int Board::tilesNeededToGetWindow(Location::Types loc) {
	int tilesNeeded = (m_stars[loc]->tile(5)) ? 5 : 0;
	tilesNeeded += (m_stars[loc]->tile(6)) ? 6 : 0;
	return tilesNeeded;
}

void Board::keepTiles(std::vector<std::shared_ptr<Tile>> to_keep) {
	if (m_keep.size() <= 4)
		m_keep = to_keep;
}

std::string Board::toString(std::shared_ptr<Location> star) {
	std::string top = "";
	std::string middle = "|1|2|3|4|5|6|";
	std::string bottom = "|";
	top += star->toString();
	for (int j = 0; j < 6; j++) {
		if (star->tile(j)) {
			bottom += "X|";
		} else {
			bottom += " |";
		}
	}
	return top + "\n" + middle + "\n" + bottom + "\n";
}

std::string Board::toString() {
	std::string top = "";
	std::string middle = "";
	std::string bottom = "|";
	for (std::shared_ptr<Location> star : m_stars) {
		top += "|" + star->toString();
		int numspaces = 11 - star->toString().size();
		for (;numspaces > 0; numspaces--) {
			top += " ";
		}
		middle += "|1|2|3|4|5|6";
		for (int j = 0; j < 6; j++) {
			if (star->tile(j)) {
				bottom += "X|";
			} else {
				bottom += " |";
			}
		}
	}
	middle += "|";
	return top + "\n" + middle + "\n" + bottom + "\n\n";
}
