#ifndef HUMANCOMMANDLINE
#define HUMANCOMMANDLINE

#include "players/Player.hpp"
#include <SFML/System/Vector2.hpp>

class Human : public Player {
public:
	Human(PlayerColour colour, std::shared_ptr<Bag> bag, std::unique_ptr<Board> board) : Player(colour, bag, std::move(board)) {};
	~Human() = default;

	// Decision overrides
	virtual PickingChoice pickTile(
		std::vector<std::shared_ptr<Factory>> factories,
		std::shared_ptr<Factory> centre,
		Tile bonus,
		bool centrePoison
	) override;
	virtual PlacingChoice placeTile(Tile bonus) override;
	virtual std::vector<std::shared_ptr<Tile>> chooseBonusPieces(std::vector<std::shared_ptr<Tile>> choices) override;
	virtual void discardDownToFour() override;
	virtual bool isAI() override {
		return false; 
	}
	virtual std::string playerTypeString() override {
		return "Human";
	}
private:
};

#endif
