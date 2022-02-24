#include "players/Human.hpp"
#include "utils/Choices.hpp"
#include <iostream>

PickingChoice Human::pickTile(
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
		for (unsigned int i = 0; i < choices.size(); ++i) {
			std::cout << i << ". " << choices[i].to_string() << "\n";
		}
		unsigned int i = 0;
		std::cin >> i;
		if (i >= 0 || i < choices.size()) {
			return choices[i];
		}
	}
}

PlacingChoice Human::placeTile(Tile bonus) {
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
		std::vector<PlacingChoice> filteredChoices = PlacingChoice::filterChoicesFromLocation(choices, star_choice->colour());
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

std::vector<std::shared_ptr<Tile>> Human::chooseBonusPieces(std::vector<std::shared_ptr<Tile>> choices, int number) {
	std::vector<std::shared_ptr<Tile>> return_list;
	while (number > 0) {
		std::cout << "\nChoose " << number << " bonus pieces\n";
		std::vector<std::shared_ptr<Tile>> availableColours;
		for (std::shared_ptr<Tile> c : choices) {
			if (std::find(availableColours.begin(), availableColours.end(), c) == availableColours.end()) {
				availableColours.push_back(c);
			}
		}
		// Could do this differently, but oh well
		for (std::shared_ptr<Tile> c : availableColours) {
			std::cout << c->toString();
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

std::vector<std::shared_ptr<Tile>> Human::discardDownToFour() {
	std::vector<std::shared_ptr<Tile>> tilesToLose;
	int num = numTiles();
	while (num > 4) {
		std::cout << "Discard tiles until four are left\n";
		std::vector<TileType> allTiles = Tile::all_tile_types();
		for (TileType tile : allTiles) {
			std::cout << Tile::toString(tile) << ": " << howManyColourStored(tile, m_stored_tiles) << "\n";
		}
		int i = -1;
		std::cin >> i;
		TileType chosenColour = (TileType)i;
		if (i >= 0 && i < allTiles.size() && howManyColourStored(chosenColour, m_stored_tiles) > 0) {
			// acceptable choice
			for (auto tile : m_stored_tiles) {
				if (tile->colour() == chosenColour) {
					tilesToLose.push_back(tile);
					break;
				}
			}
			num--;
		} else {
			std::cout << "Invalid choice, try again\n";
		}
	}
	return tilesToLose;
}
