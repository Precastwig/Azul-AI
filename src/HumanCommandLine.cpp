#include "players/HumanCommandLine.hpp"
#include <iostream>

PickingChoice HumanCommandLine::pickTile(
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
	factories.push_back(centre);
	while (1) {
		int chosen_factory = -1;
		std::cout << "Choose a factory\n";
		while (chosen_factory < 0 || chosen_factory > factories.size()) {
			// + 1 for the centre
			std::cin >> chosen_factory;
		}
		std::cout << "Choose a colour\n";
		for (int i = 0; i < tile_strings.size() - 1; i++) {
			std::cout << i << ". " << tile_strings[i] << "\n";
		}
		int chosen_colour_int = -1;
		while (chosen_colour_int < 0 || chosen_colour_int > 5) {
			std::cin >> chosen_colour_int;
		}
		Tile chosen_colour = (Tile)chosen_colour_int;
		for (PickingChoice choice : choices) {
			if (choice.factory == factories[chosen_factory] &&
				choice.tile == chosen_colour) {
				return choice;
			}
		}
	}
}

PlacingChoice HumanCommandLine::placeTile(Tile bonus) {
	std::cout << m_board.toString();
	std::vector<PlacingChoice> choices = getAllowedPlacingChoices(bonus);
	if (choices.size() == 0 || m_done_placing) {
		if (!m_done_placing) {
			std::cout << "You have no more placing choices\n";
		}
		// You have to finish placing
		m_done_placing = true;
		return PlacingChoice();
	}
	std::vector<Location> location_choices = getLocationsFromChoiceList(choices);
	while (1) {
		std::cout << "Choose a location to place:\n";
		for (int i = 0; i < location_choices.size(); i++) {
			std::cout << i << ". " << location_strings[location_choices[i]] << "\n";
		}
		int star_choice = -1;
		std::cin >> star_choice;
		if (star_choice < 0 || star_choice >= location_choices.size()) {
			std::cout << "Not a choice\n";
			continue;
		}

		std::cout << "Choose a placement\n" << m_board.toString((Location)star_choice);
		int num_choice = -1;
		std::cin >> num_choice;
		for (PlacingChoice choice : choices) {
			if (choice.star == star_choice && choice.index == num_choice) {
				return choice;
			}
		}
	}
}

std::vector<Tile> HumanCommandLine::chooseBonusPieces(std::vector<Tile> choices, int number) {
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

std::vector<Tile> HumanCommandLine::discardDownToFour() {

}
