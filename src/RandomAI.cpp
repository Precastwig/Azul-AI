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

std::vector<Tile> RandomAI::chooseBonusPieces(std::vector<Tile> choices, int number) {
	std::vector<Tile> return_list;
	std::vector<int> chosen_ints;
	while (number > 0) {
		int randomChoice = rand() % choices.size();
		if (std::find(chosen_ints.begin(), chosen_ints.end(), randomChoice) == chosen_ints.end()) {
			// Element not found
			chosen_ints.push_back(randomChoice);
			return_list.push_back(choices[randomChoice]);
			number--;
		}
	}
}
