#ifndef HUMANCOMMANDLINE
#define HUMANCOMMANDLINE

#include "players/Player.hpp"

class HumanCommandLine : public Player {
public:
	HumanCommandLine(PlayerColour colour, std::shared_ptr<Bag> bag) : Player(colour, bag) {};
	~HumanCommandLine() = default;

	// Decision overrides
	virtual PickingChoice pickTile(
		std::vector<std::shared_ptr<Factory>> factories,
		std::shared_ptr<Factory> centre,
		Tile bonus,
		bool centrePoison
	) override;
	virtual PlacingChoice placeTile(Tile bonus) override;
	virtual std::vector<Tile> chooseBonusPieces(std::vector<Tile> choices, int num) override;
	virtual std::vector<Tile> discardDownToFour() override;
private:
};

#endif
