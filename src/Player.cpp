#include "players/Player.hpp"
#include <algorithm>

Player::Player(PlayerColour colour, Game* game)
	:  m_done_placing(false), m_board(), m_game(game), m_col(colour), m_points(0) {
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
	Tile bonus
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
			PickingChoice choice;
			if (factory->isOnlyBonus(bonus)) {
				// Then picking the bonus is alllowed
				choice.with_bonus = true;
				choice.factory = factory;
				choice.tile = NONE;
				choices.push_back(choice);
			} else {
				if (tile != bonus &&
					std::find(addedFromFactory.begin(), addedFromFactory.end(), tile) == addedFromFactory.end()) {
					// We add
					addedFromFactory.push_back(tile);
					choice.with_bonus = factory->hasBonus(bonus);
					choice.factory = factory;
					choice.tile = tile;
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
			newChoice.cost.num_colour = correctColourCost;
			return_list.push_back(newChoice);
		}
	}
}

std::vector<PlacingChoice> Player::getAllowedPlacingChoices(Tile bonus) {
	std::vector<PlacingChoice> all_choices = m_board.getAllPlacingChoices();
	std::vector<PlacingChoice> valid_choices;
	for (PlacingChoice choice : all_choices) {
		std::vector<Tile> potentialCostColours;
		if (choice.cost.colour == NONE) {
			potentialCostColours = m_board.getUnusedColoursInCentre();
		} else {
			potentialCostColours.push_back(choice.cost.colour);
		}

		for (Tile costColour : potentialCostColours) {
			int max_bonus = 0;
			if (costColour != bonus) {
				max_bonus = m_num_of_each_tile[bonus];
			}

			choice.cost.colour = costColour;

			int num_relevent_colour = m_num_of_each_tile[costColour];
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

void Player::resolvePlacingChoice(PlacingChoice choice, Tile bonus, Bag bag) {
	if (!m_done_placing) {
		// Remove the cost from current number of tiles
		m_num_of_each_tile[choice.cost.colour] -= choice.cost.num_colour;
		m_num_of_each_tile[bonus] -= choice.cost.num_bonus;
		// Put the used tiles in the bin
		bag.toBin(choice.cost.colour, choice.cost.num_colour - 1);
		bag.toBin(bonus, choice.cost.num_bonus);
		// Place the tile on the board (this scores us points)
		m_board.placeTile(choice, this);
	} else {
		// Remove the tiles down to 4
		int total = 0;
		// Magic number, it's the number of tiles
		// There should be some choice being made here, and this logic
		// soup should be in a function
		for (int i = 0; i < 6; i++) {
			if (total >= 4) {
				m_num_of_each_tile[i] = 0;
			} else {
				int num = m_num_of_each_tile[i];
				if (num + total > 4) {
					// Save some
					m_num_of_each_tile[i] = 4 - total;
					total = 4;
				} else {
					total += num;
				}
			}
		}
	}
}

void Player::resolvePickingChoice(
	PickingChoice choice,
	Tile bonus,
	std::shared_ptr<Factory> centre) {
	// Add to our counts
	if (choice.factory->hasBonus(bonus)) {
		m_num_of_each_tile[bonus]++;
	}

	if (choice.tile != NONE) {
		// This part should probably be a function on the PickingChoice object
		// Oh well
		m_num_of_each_tile[choice.tile] += choice.factory->numberOf(choice.tile);
		// Remove from factory
	}
	choice.factory->removeTiles(choice.tile, bonus, centre);
}

std::string Player::toString() {
	std::string str = "Player: " + player_colour_strings[m_col] + "\n";
	for (int i = 0; i < 6; i++) {
		str += tile_strings[i] + ": " + std::to_string(m_num_of_each_tile[i]) + "\n";
	}
	if (m_done_placing) {
		str += "\nFinished placing\n";
	} else {
		str += "\n" + m_board.toString();
	}
	return str;
}
