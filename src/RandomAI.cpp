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
		bonus.colour()
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

std::vector<std::shared_ptr<Tile>> RandomAI::chooseBonusPieces(std::vector<std::shared_ptr<Tile>> choices, int number) {
	std::cout << "\nChoosing bonus pieces!\n";
	std::string s;
	std::cin >> s;
	std::vector<std::shared_ptr<Tile>> return_list;
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
	return return_list;
}

std::vector<std::shared_ptr<Tile>> RandomAI::discardDownToFour() {
	std::vector<std::shared_ptr<Tile>> return_list;
	// Keep saving tiles until we have either saved 4 or run out
	while (return_list.size() != 4 && !m_stored_tiles.empty()) {
		int keepIndex = rand() % m_stored_tiles.size();
		return_list.push_back(m_stored_tiles[keepIndex]);
	}
	return return_list;
}
