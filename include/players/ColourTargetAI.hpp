#ifndef COLOUR_TARGET_AI
#define COLOUR_TARGET_AI

#include "players/Player.hpp"

// A player object that attempts to collect a single colour
// As secondary objectives it collects both adjacant colours
// and attemtps to fill the centre star in the relevent spots
class ColourTargetAI : public Player {
public:
    ColourTargetAI(PlayerColour colour, std::shared_ptr<Bag> bag, Tile::Type target);
    ~ColourTargetAI() = default;

    // Overrides from Player
    virtual PickingChoice pickTile(
		std::vector<std::shared_ptr<Factory>> factories,
		std::shared_ptr<Factory> centre,
		Tile bonus,
		bool centrePoison
	) override;
	virtual PlacingChoice placeTile(Tile bonus) override;
	virtual std::vector<std::shared_ptr<Tile>> chooseBonusPieces(std::vector<std::shared_ptr<Tile>> choices, int num) override;
	// Returns which tiles the player would like to keep
	virtual std::vector<std::shared_ptr<Tile>> discardDownToFour() override;
	virtual bool isAI() override {
		return true;
	}
private:
    std::vector<double> generatePickingWeightsFromBoard();
	std::vector<double> generatePickingWeightsFromBoardImpl(std::vector<double>* currentWeights);
    double evaluatePickingChoice(PickingChoice choice, Tile::Type bonusCol);
    std::vector<double> m_tile_picking_weights;
    Tile::Type m_target_colour;
};

#endif