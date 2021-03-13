#ifndef RANDOM_CHOICE_AI
#define RANDOM_CHOICE_AI

#include "players/Player.hpp"

// A player object that just makes random decisions
class RandomAI : public Player {
public:
	RandomAI(PlayerColour colour, Game* game) : Player(colour, game) {
		srand (time(NULL));
	};
	~RandomAI() = default;

	// Decision overrides
	virtual PickingChoice pickTile(
		std::vector<std::shared_ptr<Factory>> factories,
		std::shared_ptr<Factory> centre,
		Tile bonus,
		bool centrePoison
	) override;
	virtual PlacingChoice placeTile(Tile bonus) override;
	virtual void pickBonusPieces(int number) override;
private:
};

#endif
