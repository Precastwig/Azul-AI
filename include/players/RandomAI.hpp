#ifndef RANDOM_CHOICE_AI
#define RANDOM_CHOICE_AI

#include "players/Player.hpp"

// A player object that just makes random decisions
class RandomAI : public Player {
public:
	RandomAI(PlayerColour colour, std::shared_ptr<Bag> bag) : Player(colour, bag) {
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
	virtual std::vector<Tile> chooseBonusPieces(std::vector<Tile> choices, int num) override;
	// Returns which tiles the player would like to keep
	virtual std::vector<Tile> discardDownToFour() override;
private:
};

#endif
