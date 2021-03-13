#include "players/RandomAI.hpp"

PickingChoice RandomAI::pickTile(
	std::vector<Factory*> factories,
	Factory* centre,
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

void RandomAI::placeTile() {
	std::vector<PlacingChoice> choices = getAllowedPlacingChoices();
	int random_index = rand() % choices.size();
	m_board.placeTile(choices[random_index], this);
}

void RandomAI::pickBonusPieces(int number) {

}
