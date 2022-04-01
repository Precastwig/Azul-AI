#include "game_elements/Board.hpp"
#include "players/Player.hpp"
#include "utils/helper_enums.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>
#include <game_elements/Location.hpp>

extern GameState g_visual_state;

Board::Board(sf::Vector2f position)
{
	// Initialise lists
	double rotation = (11 * M_PI) / 6;
	double spin = (8 * M_PI) / 6;
	double incrementval = (2* M_PI) / (Location::all_locations().size() - 1);
	for (LocationType type : Location::all_locations()) {
		auto star = std::make_shared<Location>(type);
		sf::Vector2f starPos;
		if (type == LocationType::CENTRE_STAR) {
			starPos = position;
			rotation = (9 * M_PI) / 6;
		} else {
			starPos = Factory::calculateNewPos(position, 195, spin);
		}
		star->setPosition(starPos);
		star->setRotation(rotation);
		rotation += incrementval;
		spin += incrementval;
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

	m_background = sf::RectangleShape(sf::Vector2f(600,600));
	m_background.setOrigin(sf::Vector2f(
		m_background.getSize().x / 2.0,
		m_background.getSize().y / 2.0
	));
	m_background.setPosition(position);
	m_background.setFillColor(sf::Color::White);
}

void Board::onHover(int xpos, int ypos, Game& game) {
	for (std::shared_ptr<Location> loc : m_stars) {
		loc->onHover(xpos, ypos, game);
	}
}

void Board::onClick(int x, int y, Game& game) {
	for (auto loc : m_stars) {
		loc->onClick(x, y, game);
	}
}

void Board::onLeft(TileType bonus) {
	for (std::shared_ptr<Location> loc : m_stars) {
		loc->onLeft(bonus);
	}
}

void Board::onRight(TileType bonus) {
	for (std::shared_ptr<Location> loc : m_stars) {
		loc->onRight(bonus);
	}
}

void Board::draw (sf::RenderTarget &target, sf::RenderStates states) const {
	target.draw(m_background, states);	
	// Draw each "star" of placing choices, they're not really placing choices
	for (std::shared_ptr<Location> loc : m_stars) {
		loc->draw(target, states);
	}
	
	// Draw windows/statues/columns


	// on hover, show all placing choices for that space,
	// Then on click, bring that submenu up permanently
	// Then allow user to click on each of them
}

std::vector<TileType> Board::getAdjacentStarColours(TileType starCol) {
	std::vector<TileType> returnList;
	if (starCol == TileType::ORANGE) {
		returnList.push_back(TileType::PURPLE);
	} else {
		returnList.push_back((TileType)(starCol-1));
	}

	if (starCol == TileType::PURPLE) {
		returnList.push_back(TileType::ORANGE);
	} else {
		returnList.push_back((TileType)(starCol+1));
	}
	return returnList;
}

bool check_column_relevent_centre_spaces(LocationType loc, std::shared_ptr<Location> centre) {
	switch(loc) {
		case CENTRE_STAR:
			// this makes no sense
			return false;
		case RED_STAR:
			return centre->tile(0) && centre->tile(1);
		case BLUE_STAR:
			return centre->tile(1) && centre->tile(2);
		case YELLOW_STAR:
			return centre->tile(2) && centre->tile(3);
		case GREEN_STAR:
			return centre->tile(3) && centre->tile(4);
		case PURPLE_STAR:
			return centre->tile(4) && centre->tile(5);
		case ORANGE_STAR:
			return centre->tile(5) && centre->tile(0);
	}
	return false;
}

void Board::setTempVisualState() {
	m_background.setFillColor(Colours::change_opacity(m_background.getFillColor(), 100));
}

void Board::setRegularVisualState() {
	m_background.setFillColor(Colours::change_opacity(m_background.getFillColor(), 255));
}

int Board::bonusPiecesAwarded() {
	// There should only be one change every time we call this
	// function, so we can return when we find a true
	for (LocationType location : Location::all_locations_except_centre()) {
		std::shared_ptr<Location> star = m_stars[location];
		if (!m_windows[location]) {
			// If the window is covered
			if (star->tile(4) &&
				star->tile(5)) {
				m_windows[location] = true;
				return 3;
			}
		}

		if (!m_statues[location]) {
			// If the statue is covered
			if (star->tile(0) && star->tile(1)) {
				// We also need to check the "next one"
				LocationType next_location = Location::clockwise_location(location);
				std::shared_ptr<Location> nextStar = m_stars[next_location];
				if(nextStar->tile(3) && nextStar->tile(2)) {
					m_statues[location] = true;
					return 2;
				}
			}
		}
		if (!m_columns[location]) {
			// If the column is covered
			if (star->tile(1) && star->tile(2)) {
				// We also need to check if the central pieces are covered
				std::shared_ptr<Location> centralStar = m_stars[LocationType::CENTRE_STAR];
				if (check_column_relevent_centre_spaces(location, centralStar)) {
					m_columns[location] = true;
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

std::vector<TileType> Board::getUnusedColoursInCentre() {
	return m_colours_not_in_centre;
}

void Board::placeTile(PlacingChoice choice, Player* me) {
	// Place the tile
	choice.star->place(choice.index, choice.cost.colour);

	if (choice.star->colour() == LocationType::CENTRE_STAR) {
		// We need to log what colour we're placing
		m_colours_not_in_centre.erase(std::remove(m_colours_not_in_centre.begin(), m_colours_not_in_centre.end(), choice.cost.colour));
	}

	// Count points gained
	int points = choice.points_gained();
	points += bonusPointsAwarded();

	// Give the player their points
	me->addPoints(points);

	// Assertain whether we need to allot bonus piece
	int reward = bonusPiecesAwarded();
	if (reward > 0) {
		// and then reward them with some bonus tiles
		me->setBonusToPick(reward);
		g_visual_state.set_reward();
	}
}

std::vector<PlacingChoice> Board::getPlacingChoicesOfCol(TileType col) {
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

int Board::tilesNeededToGetStatue(LocationType star) {
	// Check 3/4 of tilcol
	int tilesNeededForColumn = (m_stars[star]->tile(0)) ? 1 : 0;
	tilesNeededForColumn += (m_stars[star]->tile(1)) ? 2 : 0;
	if (tilesNeededForColumn == 0) {
		return 0;
	}
	LocationType nextStar = Location::clockwise_location(star);
	tilesNeededForColumn += (m_stars[nextStar]->tile(2)) ? 3 : 0;
	tilesNeededForColumn += (m_stars[nextStar]->tile(3)) ? 4 : 0;
	return tilesNeededForColumn;
}

int Board::tilesNeededToGetWindow(LocationType loc) {
	int tilesNeeded = (m_stars[loc]->tile(4)) ? 5 : 0;
	tilesNeeded += (m_stars[loc]->tile(5)) ? 6 : 0;
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
