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
#include <players/PlayerInfo.hpp>

extern GameState g_visual_state;
extern PlayerInfo g_player_info;

Board::Board(sf::Vector2f position, sf::Vector2f total_size, bool display_bonus_right, int bonus_size) : m_display_bonus_on_right(display_bonus_right)
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

	// Intitialise tile bonuses
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

	int height = total_size.y;
	int width = total_size.x - bonus_size;
	m_background = sf::RectangleShape(sf::Vector2f(width,height));
	m_background.setOrigin(sf::Vector2f(
		m_background.getSize().x / 2.0,
		m_background.getSize().y / 2.0
	));
	m_background.setPosition(position);
	m_background.setFillColor(sf::Color::White);

	m_bonus_background = sf::RectangleShape(sf::Vector2f(bonus_size, height));
	m_bonus_background.setOrigin(sf::Vector2f(
		m_bonus_background.getSize().x / 2.0,
		m_bonus_background.getSize().y / 2.0
	));
	int bonus_bg_x_pos = (m_display_bonus_on_right) ? 
		position.x + (width / 2.0) + (bonus_size / 2.0) :
		position.x - (width / 2.0) - (bonus_size / 2.0);
	m_bonus_background.setPosition(sf::Vector2f(
		bonus_bg_x_pos,
		position.y
	));
	m_bonus_background.setFillColor(sf::Color(61,61,61, 255));

	double bonus_point_tile_spacing = 5.0;
	int num_bonus_point_tiles = 11;
	int num_spacings = num_bonus_point_tiles + 1;
	double bonus_point_tile_height = (height - (num_spacings * bonus_point_tile_spacing)) / (num_bonus_point_tiles * 2.0);
	// Work out tile size
	sf::Vector2f bonus_point_tile_size(
		bonus_point_tile_height * (5.0 / 3.0),
		bonus_point_tile_height
	);
	sf::Vector2f bonus_point_tile_pos(
		bonus_bg_x_pos,
		position.y - height / 2.0 + bonus_point_tile_size.y + bonus_point_tile_spacing
	);
	auto next_bonus_tile_pos = [bonus_point_tile_spacing, bonus_point_tile_size](sf::Vector2f& position) {
		position.y += bonus_point_tile_spacing + (bonus_point_tile_size.y * 2);
	};
	for (size_t i = 1; i <= 4; ++i) {
		// Make the 1-4 bonus score tiles
		std::shared_ptr<Tile> new_t = std::make_shared<Tile>(
			TileType::NONE, bonus_point_tile_size
		);
		new_t->setRotation(0.0);
		new_t->setPosition(bonus_point_tile_pos);
		new_t->setFillColor(sf::Color(153,153,153,100));
		new_t->setOutlineColor(sf::Color(255,255,255,100));
		new_t->setOutlineThickness(1.0);

		m_bonus_point_nums.push_back(new_t);
		sf::Text new_text;
		new_text.setFont(g_font);
		new_text.setCharacterSize(20);
		new_text.setString(std::to_string(i));
		sf::FloatRect playerRect = new_text.getLocalBounds();
    	new_text.setOrigin(playerRect.left + playerRect.width/2.0f,
                        playerRect.top  + playerRect.height/2.0f);
		new_text.setColor(sf::Color::Black);
		new_text.setPosition(bonus_point_tile_pos);
		m_bonus_point_nums_nums.push_back(new_text);
		next_bonus_tile_pos(bonus_point_tile_pos);
	}
	for (auto star : m_stars) {
		std::shared_ptr<Tile> new_t = std::make_shared<Tile>(
			star->get_tile_type(), bonus_point_tile_size
		);
		new_t->setRotation(0.0);
		new_t->setPosition(bonus_point_tile_pos);
		new_t->setOutlineColor(sf::Color(255,255,255,100));
		new_t->setOutlineThickness(1.0);
		if (new_t->colour() == NONE) {
			// None doesn't have a default colour to take advantage of
			new_t->setFillColor(sf::Color(200,200,200,100));
		}
		new_t->setOpacity(100);

		next_bonus_tile_pos(bonus_point_tile_pos);
		m_bonus_point_colours.push_back(new_t);
	}
}

void Board::setHoverTextStrAndPos(sf::String str, float xpos, float ypos) {
	if (m_hover_text) {
		m_hover_text->setPosition(xpos + 15, ypos + 15);
	} else {
		m_hover_text = std::make_shared<sf::Text>();
		m_hover_text->setFont(g_font);
		m_hover_text->setCharacterSize(20);
		m_hover_text->setColor(sf::Color::Green);
		m_hover_text->setString(str);
		m_hover_text->setPosition(xpos + 15, ypos + 15);
	}
}

void Board::onHover(int xpos, int ypos, Game& game) {
	for (std::shared_ptr<Location> loc : m_stars) {
		loc->onHover(xpos, ypos, game);
	}
	if (g_visual_state.is_placing()) {
		bool none = true;
		for (size_t i = 0 ; i < m_bonus_point_nums.size(); ++i) {
			if (m_bonus_point_nums[i]->contains(xpos, ypos)) {
				setHoverTextStrAndPos("+" + std::to_string(all_nums_points[i]), xpos, ypos);
				return;
			}
		}
		for (size_t i = 0; i < m_bonus_point_colours.size(); ++i) {
			if (m_bonus_point_colours[i]->contains(xpos, ypos)) {
				setHoverTextStrAndPos("+" + std::to_string( m_stars[i]->pointsForFill()), xpos, ypos);
				return;
			}
		}
		// If we reach here then we need to remove the hover text
		m_hover_text = nullptr;
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
	target.draw(m_bonus_background, states);
	for (std::shared_ptr<Tile> t : m_bonus_point_nums) {
		target.draw(*t, states);
	}
	for (auto& n : m_bonus_point_nums_nums) {
		target.draw(n, states);
	}
	for (std::shared_ptr<Tile> t : m_bonus_point_colours) {
		target.draw(*t, states);
	}
	// Draw each "star" of placing choices, they're not really placing choices
	for (std::shared_ptr<Location> loc : m_stars) {
		loc->draw(target, states);
	}
	
	// Draw windows/statues/columns

	// Draw hover text
	if (m_hover_text) {
		target.draw(*m_hover_text, states);
	}
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
	for (size_t i = 0; i < m_stars.size(); ++i) {
		std::shared_ptr<Location> star = m_stars[i];
		if (star->full() && !star->isScoredFillPoints()) {
			points += star->pointsForFill();
			star->scoredFillPoints();
			g_logger.log(Logger::INFO, "Adding points for filling star");
			m_bonus_point_colours[i]->setOpacity(255);
			m_bonus_point_colours[i]->setOutlineThickness(3.0);
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
				m_bonus_point_nums[i]->setOpacity(255);
				m_bonus_point_nums[i]->setOutlineThickness(3.0);
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
