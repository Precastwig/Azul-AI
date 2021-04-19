#include "players/HumanCommandLine.hpp"
#include <iostream>
#include <set>

PickingChoice HumanCommandLine::pickTile(
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
	factories.push_back(centre);
	while (1) {
		// int chosen_factory = -1;
		// std::cout << "Choose a factory\n";
		// while (chosen_factory < 0 || chosen_factory > factories.size()) {
		// 	// + 1 for the centre
		// 	std::cin >> chosen_factory;
		// }
		// std::cout << "Choose a colour\n";
		// for (int i = 0; i < tile_strings.size() - 1; i++) {
		// 	std::cout << i << ". " << tile_strings[i] << "\n";
		// }
		// int chosen_colour_int = -1;
		// while (chosen_colour_int < 0 || chosen_colour_int > 5) {
		// 	std::cin >> chosen_colour_int;
		// }
		// Tile chosen_colour = (Tile)chosen_colour_int;
		// for (PickingChoice choice : choices) {
		// 	if (choice.factory == factories[chosen_factory] &&
		// 		choice.tile == chosen_colour) {
		// 		return choice;
		// 	}
		// }
		for (unsigned int i = 0; i < choices.size(); ++i) {
			std::cout << i << ". " << choices[i].tile.toString() << " factory " << choices[i].factory->id() << "\n";
		}
		unsigned int i = 0;
		std::cin >> i;
		if (i >= 0 || i < choices.size()) {
			return choices[i];
		}
	}
}

PlacingChoice HumanCommandLine::placeTile(Tile bonus) {
	std::cout << toString();
	std::vector<PlacingChoice> choices = getAllowedPlacingChoices(bonus);
	if (choices.size() == 0 || m_done_placing) {
		if (!m_done_placing) {
			std::cout << "You have no more placing choices\n";
		}
		// You have to finish placing
		m_done_placing = true;
		return PlacingChoice();
	}
	std::vector<std::shared_ptr<Location>> location_choices = getLocationsFromChoiceList(choices);
	while (1) {
		std::cout << "Choose a location to place:\n";
		for (unsigned int i = 0; i < location_choices.size(); i++) {
			std::cout << i << ". " << location_choices[i]->toString() << "\n";
		}
		unsigned int star_choice_index = -1;
		std::cin >> star_choice_index;
		if (star_choice_index < 0 || star_choice_index >= location_choices.size()) {
			std::cout << "Not a choice\n";
			continue;
		}

		std::shared_ptr<Location> star_choice = location_choices[star_choice_index];

		std::cout << "Choose a placement:\n" << m_board.toString(star_choice);
		std::vector<PlacingChoice> filteredChoices = filterChoicesFromLocation(choices, star_choice);
		for (PlacingChoice choice : choices) {
			if (choice.star == star_choice) {
				std::cout << choice.index.getIndex() + 1 << ", col:" << choice.cost.num_colour << ", bonus:" << choice.cost.num_bonus << "\n";
			}
		}
		std::cout << "\n";
		unsigned int num_choice_index = -1;
		std::cin >> num_choice_index;
		if (num_choice_index < 0 || num_choice_index >= filteredChoices.size()) {
			std::cout << "Not a choice\n";
			continue;
		}
		return filteredChoices[num_choice_index];
	}
}

std::vector<Tile> HumanCommandLine::chooseBonusPieces(std::vector<Tile> choices, int number) {
	std::vector<Tile> return_list;
	while (number > 0) {
		std::cout << "\nChoose " << number << " bonus pieces\n";
		std::vector<Tile> availableColours;
		for (Tile c : choices) {
			if (std::find(availableColours.begin(), availableColours.end(), c) == availableColours.end()) {
				availableColours.push_back(c);
			}
		}
		// Could do this differently, but oh well
		for (Tile c : availableColours) {
			std::cout << c.toString();
		}
		int i = -1;
		std::cin >> i;
		if (i >= 0 && i < availableColours.size()) {
			return_list.push_back(availableColours[i]);
			number--;
		}
	}
	return return_list;
}

std::vector<Tile> HumanCommandLine::discardDownToFour() {
	std::vector<Tile> tilesToLose;
	int num = numTiles();
	while (num > 4) {
		std::cout << "Discard tiles until four are left\n";
		std::vector<Tile> allTiles = Tile::all_tiles();
		for (unsigned int i = 0; i < allTiles.size(); ++i) {
			std::cout << i << ". " << allTiles[i].toString() << " " << m_num_of_each_tile[i] << "\n";
		}
		unsigned int i = -1;
		std::cin >> i;
		if (i >= 0 && i < allTiles.size()) {
			// acceptable choice
			tilesToLose.push_back(allTiles[i]);
			num--;
		}
	}
	return tilesToLose;
}
