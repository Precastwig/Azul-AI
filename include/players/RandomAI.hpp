#ifndef RANDOM_CHOICE_AI
#define RANDOM_CHOICE_AI

#include "players/Player.hpp"

// A player object that just makes random decisions
class RandomAI : public Player {
public:
	RandomAI(PlayerColour colour, std::shared_ptr<Bag> bag, sf::Vector2f boardpos) : Player(colour, bag, boardpos) {
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
	virtual std::vector<std::shared_ptr<Tile>> chooseBonusPieces(std::vector<std::shared_ptr<Tile>> choices, int num) override;
	// Returns which tiles the player would like to keep
	virtual void discardDownToFour() override;
	virtual bool isAI() override {
		return true; 
	}
private:
};

#endif
