#include "player.hpp"
#include <algorithm>

Player::Player(PlayerColour colour, Game* game)
	: m_board(), m_game(game), m_col(colour), m_points(0) {
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
	std::vector<Factory*> factories,
	Factory* centre,
	Tile bonus
)
{
	std::vector<PickingChoice> choices;
	// This function doesn't care about the difference between the middle and
	// the other factories
	factories.push_back(centre);
	// Loop through factories
	for (Factory* factory : factories) {
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
}

std::vector<PlacingChoice> Player::getAllowedPlacingChoices() {
	std::vector<PlacingChoice> allchoices = m_board.getAllPlacingChoices();
}

void Player::resolvePickingChoice(
	PickingChoice choice,
	Tile bonus,
	Factory* centre) {
	// Add to our counts
	if (choice.factory->hasBonus(bonus)) {
		m_num_of_each_tile[bonus]++;
	}

	// This part should probably be a function on the PickingChoice object
	// Oh well
	m_num_of_each_tile[choice.tile] += choice.factory->numberOf(choice.tile);
	// Remove from factory
	choice.factory->removeTiles(choice.tile, bonus, centre);
}
