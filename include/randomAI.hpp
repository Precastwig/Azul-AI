#ifndef RANDOM_CHOICE_AI
#define RANDOM_CHOICE_AI

#include "player.hpp"

// A player object that just makes random decisions
class RandomAI : public Player {
public:
	RandomAI(PlayerColour colour, Game* game) : Player(colour, game) {};
	~RandomAI() = default;

	// Decision overrides
	virtual PickingChoice pickTile(
		std::vector<Factory*> factories,
		Factory* centre,
		Tile bonus,
		bool centrePoison
	) override;
	virtual void placeTile() override;
	virtual void pickBonusPieces(int number) override;
private:
};

#endif
