#include "players/Player.hpp"
#include "Game.hpp"
#include "utils/helper_enums.hpp"
#include <SFML/System/Vector2.hpp>
#include <exception>
#include <players/PlayerInfo.hpp>
#include <algorithm>
#include <memory>
#include <set>
#include <iostream>
#include <chrono>
#include <thread>

extern PlayerInfo g_player_info;

Player::Player(PlayerColour playercolour, std::shared_ptr<Bag> bag, sf::Vector2f boardpos)
	:  m_done_placing(false), m_discarded(false), m_board(boardpos), m_bag(bag), m_col(playercolour), m_points(0), m_bonus_to_choose(0), m_stored_from_previous_round(0) {
};

void Player::addPoints(int points) {
	m_points += points;
}

bool Player::hasTiles() {
	return !m_stored_tiles.empty();
}

std::vector<PickingChoice> Player::getAllPickingChoices(
	std::vector<std::shared_ptr<Factory>> factories,
	std::shared_ptr<Factory> centre,
	TileType bonus
)
{
	std::vector<PickingChoice> choices;
	// This function doesn't care about the difference between the middle and
	// the other factories
	factories.push_back(centre);
	// Loop through factories
	for (std::shared_ptr<Factory> factory : factories) {
		std::vector<std::shared_ptr<Tile>> addedFromFactory;
		for (std::shared_ptr<Tile> tile : factory->tiles()) {
			// Don't add bonus tiles or tile types that have already been added
			// (for this factory)
			if (factory->isOnlyBonus(bonus)) {
				PickingChoice choice(TileType::NONE);
				// Then picking the bonus is alllowed
				choice.with_bonus = true;
				choice.factory = factory;
				choices.push_back(choice);
			} else {
				if (tile->colour() != bonus &&
					std::find(addedFromFactory.begin(), addedFromFactory.end(), tile) == addedFromFactory.end()) {
					// We add
					addedFromFactory.push_back(tile);
					PickingChoice choice(tile->colour());
					choice.with_bonus = factory->hasBonus(bonus);
					choice.factory = factory;
					choices.push_back(choice);
				}
			}
		}
	}
	return choices;
}

void Player::sortTiles() {
	std::sort(m_stored_tiles.begin(), m_stored_tiles.end(), [](
		std::shared_ptr<Tile>& tile_ptr1,
		std::shared_ptr<Tile>& tile_ptr2
	) {
		return tile_ptr1->colour() > tile_ptr2->colour();
	});
}

void Player::highlightCostTiles(const PlacingChoice& choice, TileType bonus) {
	// Reset all highlights
	if (!choice.star) {
		g_logger.log(Logger::INFO, "Reset cost highlight");
		for (auto& tile : m_stored_tiles) {
			tile->setOutlineColor(sf::Color::Black);
			tile->setOutlineThickness(0.0);
		}
		return;
	}

	g_logger.log(Logger::INFO, "Highlighting cost choice: " + choice.to_string());
	int num_col = choice.cost.num_colour;
	int num_bon = choice.cost.num_bonus;
	for (auto& tile : m_stored_tiles) {
		if (num_col > 0 && choice.cost.colour == tile->colour()) {
			num_col--;
			tile->setOutlineColor(sf::Color::Red);
			tile->setOutlineThickness(5.0);
		} else if (num_bon > 0 && tile->colour() == bonus) {
			num_bon--;
			tile->setOutlineColor(sf::Color::Red);
			tile->setOutlineThickness(5.0);
		} else {
			tile->setOutlineColor(sf::Color::Black);
			tile->setOutlineThickness(0.0);
		}
	}
}

void Player::commandLineWait() {
	for (unsigned int i = 0; i < 5; ++i) {
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		std::cout << ".";
		fflush(stdout);
	}
	std::cout << "\n";
}

int Player::getPoisonPoints() {
	return m_stored_tiles.size() - m_stored_from_previous_round;
}

void Player::minusPoisonPoints() {
	m_points -= getPoisonPoints();
	if (m_points < 0) {
		m_points = 0;
	}
}

void Player::createAllVariationsOfChoice(
	PlacingChoice base_choice,
	std::vector<PlacingChoice>& return_list,
	int max_bonus,
	int max_relevent_colour
) {
	int cost = base_choice.cost.num_colour;
	for (int correctColourCost = max_relevent_colour; correctColourCost > 0; correctColourCost--) {
		if (correctColourCost + max_bonus >= cost) {
			// We can afford this placement
			PlacingChoice newChoice = base_choice;
			int bonus_needed = cost - correctColourCost;
			newChoice.cost.num_bonus = (bonus_needed > 0) ? bonus_needed : 0;
			if (bonus_needed >= 0) {
				newChoice.cost.num_colour = correctColourCost;
			}
			if (std::find(return_list.begin(), return_list.end(), newChoice) == return_list.end()) {
				return_list.push_back(newChoice);
			}
		}
	}
}

void Player::pickBonusPieces() {
	std::vector<std::shared_ptr<Tile>> choices = chooseBonusPieces(m_bag->rewardTiles());
	// Add the tiles to our piece list
	m_stored_tiles.insert(m_stored_tiles.end(), choices.begin(), choices.end());
	sortTiles();
	// Remove the tiles from the bonus board
	m_bag->takeRewardTiles(choices);
}

void Player::pass() {
	m_done_placing = true;
	m_discarded = true;
	m_stored_from_previous_round = m_stored_tiles.size();
	g_player_info.nextTurn();	
}

int Player::howManyColourStored(TileType t, std::vector<std::shared_ptr<Tile>> stored) {
	int count = 0;
	for (auto tile : stored) {
		if (tile->colour() == t) {
			count++;
		}
	}
	return count;
}

std::vector<PlacingChoice> Player::getAllowedPlacingChoices(Tile bonus) {
	std::vector<PlacingChoice> all_choices = m_board.getAllPlacingChoices();

	std::vector<PlacingChoice> valid_choices;
	for (PlacingChoice choice : all_choices) {
		std::vector<TileType> potentialCostColours;
		if (choice.cost.colour == TileType::NONE) {
			potentialCostColours = m_board.getUnusedColoursInCentre();
		} else {
			potentialCostColours.push_back(choice.cost.colour);
		}

		for (Tile costTile : potentialCostColours) {
			int max_bonus = 0;
			if (costTile != bonus) {
				max_bonus = howManyColourStored(bonus.colour(), m_stored_tiles);
			}

			choice.cost.colour = costTile.colour();

			int num_relevent_colour = howManyColourStored(costTile.colour(), m_stored_tiles);
			createAllVariationsOfChoice(
				choice,
				valid_choices,
				max_bonus,
				num_relevent_colour
			);
		}
	}

	return valid_choices;
}

void Player::resolvePlacingChoice(PlacingChoice& choice, TileType bonus) {
	if (!m_done_placing) {
		g_logger.log(Logger::INFO, toString() + " placed tile");
		// Remove the cost from current number of tiles
		std::vector<std::shared_ptr<Tile>> newStoredTiles;
		std::vector<std::shared_ptr<Tile>> toBin;
		bool firstTile = true;
		for (std::shared_ptr<Tile> t : m_stored_tiles) {
			if (t->colour() == choice.cost.colour && choice.cost.num_colour > 0) {
				// We're placing this tile, put it in the bin, removing the first one forever
				if (firstTile) {
					firstTile = false;
				} else {
					toBin.push_back(t);
				}
				choice.cost.num_colour--;
			} else if (t->colour() == bonus && choice.cost.num_bonus > 0) {
				choice.cost.num_bonus--;
				toBin.push_back(t);
			} else {
				// We're not placing this tile, so keep it
				newStoredTiles.push_back(t);
			}
		}
		// Keep the remaining tiles
		m_stored_tiles = newStoredTiles;
		// Put the used tiles in the bin
		m_bag->toBin(toBin);
		// Place the tile on the board (this scores us points)
		m_board.placeTile(choice, this);
	} else if (!m_discarded) {
		// Remove the tiles down to 4
		discardDownToFour();
		m_discarded = true;
		// Only do this once
	}
}

int Player::numTiles() {
	return m_stored_tiles.size();
}

std::vector<std::shared_ptr<Location>> Player::getLocationsFromChoiceList(std::vector<PlacingChoice> choices) {
	std::set<std::shared_ptr<Location>> return_list;
	for (PlacingChoice choice : choices) {
		return_list.insert(choice.star);
	}
	return std::vector<std::shared_ptr<Location>>(return_list.begin(), return_list.end());
}

void Player::resolvePickingChoice(
	PickingChoice& choice,
	TileType bonus,
	std::shared_ptr<Factory> centre) {
	if (!g_player_info.centreTaken() && choice.factory == centre) {
		// Someone has taken from the centre
		g_logger.log(Logger::INFO, "Someone has picked from centre");
		g_player_info.setCentreTaken(true);
		minusPoisonPoints();
		g_player_info.setStartingPlayer();
	}
	std::vector<std::shared_ptr<Tile>> pickedTiles = choice.factory->removeTiles(choice.tile_colour, bonus, centre);
	m_stored_tiles.insert(m_stored_tiles.end(), pickedTiles.begin(), pickedTiles.end());
	sortTiles();
}

std::string Player::toShortString() {
	return m_col.toString() + "(" + playerTypeString() + ") : " + std::to_string(m_points);
}

std::string Player::toStringNoBoard() {
	std::string str = "Player: " + m_col.toString() + " (" + playerTypeString() + ")\n";
	str += "Points: " + std::to_string(m_points) + "\n";
	for (TileType tile : Tile::all_tile_types()) {
		str += Tile::toString(tile) + ": " + std::to_string(howManyColourStored(tile, m_stored_tiles)) + "\n";
	}
	str += "Bonus to place: " + std::to_string(m_bonus_to_choose) + "\n";
	return str;
}

std::string Player::toString() {
	std::string str = toStringNoBoard();
	if (m_done_placing) {
		str += "\nFinished placing\n";
	} else {
		str += "\n" + m_board.toString();
	}
	return str;
}
