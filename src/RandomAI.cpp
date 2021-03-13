#include "players/RandomAI.hpp"
#include <iostream>

PickingChoice RandomAI::pickTile(
	std::vector<std::shared_ptr<Factory>> factories,
	std::shared_ptr<Factory> centre,
	Tile bonus,
	bool centrePoison
) {
	std::vector<PickingChoice> choices = getAllPickingChoices(
		factories,
		centre,
		bonus
	);
	int random_index = rand() % choices.size();
 	return choices[random_index];
}

PlacingChoice RandomAI::placeTile(Tile bonus) {
	std::vector<PlacingChoice> choices = getAllowedPlacingChoices(bonus);
	if (choices.size() == 0) {
		m_done_placing = true;
		return PlacingChoice();
	}
	int random_index = rand() % choices.size();
	return choices[random_index];
}

void RandomAI::pickBonusPieces(int number) {

}
