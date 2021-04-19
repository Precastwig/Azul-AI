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

std::vector<Tile> RandomAI::chooseBonusPieces(std::vector<Tile> choices, int number) {
	std::cout << "\nChoosing bonus pieces!\n";
	std::string s;
	std::cin >> s;
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
	return return_list;
}

std::vector<Tile> RandomAI::discardDownToFour() {
	std::vector<Tile> return_list;
	// Magic number, it's the number of tiles
	for (int i = 0; i < 6; i++) {
		if (return_list.size() == 4) {
			return return_list;
		} else {
			int num_kept = return_list.size();
			int num = m_num_of_each_tile[i];
			if (num + num_kept > 4) {
				// Save some, but not all
				num = 4 - num_kept;
			}
			// Save num of the current type
			for (; num > 0; num--) {
				return_list.push_back(Tile::all_tiles()[i]);
			}
		}
	}
	return return_list;
}
