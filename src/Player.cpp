#include "players/Player.hpp"
#include "Game.hpp"
#include <algorithm>
#include <set>
#include <iostream>

Player::Player(PlayerColour playercolour, std::shared_ptr<Bag> bag)
	:  m_done_placing(false), m_discarded(false), m_board(), m_bag(bag), m_col(playercolour), m_points(0) {
};

void Player::addPoints(int points) {
	m_points += points;
}

bool Player::hasTiles() {
	for (int colour : m_num_of_each_tile) {
		if (colour > 0) {
			return true;
		}
	}
	return false;
}

std::vector<PickingChoice> Player::getAllPickingChoices(
	std::vector<std::shared_ptr<Factory>> factories,
	std::shared_ptr<Factory> centre,
	Tile::Type bonus
)
{
	std::vector<PickingChoice> choices;
	// This function doesn't care about the difference between the middle and
	// the other factories
	factories.push_back(centre);
	// Loop through factories
	for (std::shared_ptr<Factory> factory : factories) {
		std::vector<Tile> addedFromFactory;
		for (Tile tile : factory->tiles()) {
			// Don't add bonus tiles or tile types that have already been added
			// (for this factory)
			if (factory->isOnlyBonus(bonus)) {
				PickingChoice choice(Tile::NONE);
				// Then picking the bonus is alllowed
				choice.with_bonus = true;
				choice.factory = factory;
				choices.push_back(choice);
			} else {
				if (tile != bonus &&
					std::find(addedFromFactory.begin(), addedFromFactory.end(), tile) == addedFromFactory.end()) {
					// We add
					addedFromFactory.push_back(tile);
					PickingChoice choice(tile);
					choice.with_bonus = factory->hasBonus(bonus);
					choice.factory = factory;
					choices.push_back(choice);
				}
			}
		}
	}
	return choices;
}

void Player::minusPoisonPoints() {
	int num_tiles = 0;
	for (int n : m_num_of_each_tile) {
		num_tiles += n;
	}
	m_points -= num_tiles;
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

void Player::pickBonusPieces(int number) {
	std::vector<Tile> choices = chooseBonusPieces(m_bag->rewardTiles(), number);
	// Add the tiles to our piece list
	for (Tile tile : choices) {
		m_num_of_each_tile[tile.colour()]++;
	}
	// Remove the tiles from the bonus board
	m_bag->takeRewardTiles(choices);
}

std::vector<PlacingChoice> Player::getAllowedPlacingChoices(Tile bonus) {
	std::vector<PlacingChoice> all_choices = m_board.getAllPlacingChoices();
	// int choice = -1;
	// std::cout << "Press 1 to output all choices\n";
	// std::cin >> choice;
	// if (choice == 1) {
	// 	for (PlacingChoice choice : all_choices) {
	// 		std::cout << location_strings[choice.star] << " " << tile_strings[choice.cost.colour] << ": " << choice.cost.num_colour << " Bonus: " << choice.cost.num_bonus << "\n";
	// 	}
	// }

	std::vector<PlacingChoice> valid_choices;
	for (PlacingChoice choice : all_choices) {
		std::vector<Tile> potentialCostColours;
		if (choice.cost.colour == Tile::NONE) {
			potentialCostColours = m_board.getUnusedColoursInCentre();
		} else {
			potentialCostColours.push_back(choice.cost.colour);
		}

		for (Tile costTile : potentialCostColours) {
			int max_bonus = 0;
			if (costTile != bonus) {
				max_bonus = m_num_of_each_tile[bonus.colour()];
			}

			choice.cost.colour = costTile.colour();

			int num_relevent_colour = m_num_of_each_tile[costTile.colour()];
			createAllVariationsOfChoice(
				choice,
				valid_choices,
				max_bonus,
				num_relevent_colour
			);
		}
	}
	// choice = -1;
	// std::cout << "Press 1 to output all filtered choices\n";
	// std::cin >> choice;
	// if (choice == 1) {
	// 	for (PlacingChoice choice : valid_choices) {
	// 		std::cout << location_strings[choice.star] << " " << tile_strings[choice.cost.colour] << ": " << choice.cost.num_colour << " Bonus: " << choice.cost.num_bonus << "\n";
	// 	}
	// }n

	return valid_choices;
}

void Player::resolvePlacingChoice(PlacingChoice choice, Tile bonus) {
	if (!m_done_placing) {
		// Remove the cost from current number of tiles
		m_num_of_each_tile[choice.cost.colour] -= choice.cost.num_colour;
		m_num_of_each_tile[bonus.colour()] -= choice.cost.num_bonus;
		// Put the used tiles in the bin
		m_bag->toBin(choice.cost.colour, choice.cost.num_colour - 1);
		m_bag->toBin(bonus, choice.cost.num_bonus);
		// Place the tile on the board (this scores us points)
		m_board.placeTile(choice, this);
	} else if (!m_discarded) {
		// Remove the tiles down to 4
		std::vector<Tile> keep = discardDownToFour();
		m_discarded = true;
		// Only do this once
		for (Tile tile : keep) {
			m_num_of_each_tile[tile.colour()]++;
		}
	}
}

int Player::numTiles() {
	int total = 0;
	for (unsigned int i = 0; i < m_num_of_each_tile.size(); i++) {
		total += m_num_of_each_tile[i];
	}
	return total;
}

std::vector<std::shared_ptr<Location>> Player::getLocationsFromChoiceList(std::vector<PlacingChoice> choices) {
	std::set<std::shared_ptr<Location>> return_list;
	for (PlacingChoice choice : choices) {
		return_list.insert(choice.star);
	}
	return std::vector<std::shared_ptr<Location>>(return_list.begin(), return_list.end());
}

std::vector<PlacingChoice> Player::filterChoicesFromLocation(std::vector<PlacingChoice> choices, std::shared_ptr<Location> location) {
	std::vector<PlacingChoice> return_list;
	for (PlacingChoice choice : choices) {
		if (choice.star == location) {
			return_list.push_back(choice);
		}
	}
	return return_list;
}

void Player::resolvePickingChoice(
	PickingChoice choice,
	Tile bonus,
	std::shared_ptr<Factory> centre) {
	// Add to our counts
	if (choice.factory->hasBonus(bonus)) {
		m_num_of_each_tile[bonus.colour()]++;
	}

	if (choice.tile != Tile::NONE) {
		// This part should probably be a function on the PickingChoice object
		// Oh well
		m_num_of_each_tile[choice.tile.colour()] += choice.factory->numberOf(choice.tile);
		// Remove from factory
	}
	choice.factory->removeTiles(choice.tile, bonus, centre);
}

std::string Player::toString() {
	std::string str = "Player: " + m_col.toString() + "\n";
	str += "Points: " + std::to_string(m_points) + "\n";
	for (Tile tile : Tile::all_tiles()) {
		str += tile.toString() + ": " + std::to_string(m_num_of_each_tile[tile.colour()]) + "\n";
	}
	if (m_done_placing) {
		str += "\nFinished placing\n";
	} else {
		str += "\n" + m_board.toString();
	}
	return str;
}
