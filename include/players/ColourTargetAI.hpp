#ifndef COLOUR_TARGET_AI
#define COLOUR_TARGET_AI

#include "players/Player.hpp"
#include "utils/helper_enums.hpp"

// A player object that attempts to collect a single colour
// As secondary objectives it collects both adjacant colours
// and attemtps to fill the centre star in the relevent spots
class ColourTargetAI : public Player {
public:
    ColourTargetAI(PlayerColour colour, std::shared_ptr<Bag> bag, sf::Vector2f boardpos, TileType target);
    ~ColourTargetAI() = default;

    // Overrides from Player
    virtual PickingChoice pickTile(
		std::vector<std::shared_ptr<Factory>> factories,
		std::shared_ptr<Factory> centre,
		Tile bonus,
		bool centrePoison
	) override;
	virtual PlacingChoice placeTile(Tile bonus) override;
	virtual std::vector<std::shared_ptr<Tile>> chooseBonusPieces(std::vector<std::shared_ptr<Tile>> choices) override;
	// Returns which tiles the player would like to keep
	virtual void discardDownToFour() override;
	virtual bool isAI() override {
		return true;
	}
	virtual std::string playerTypeString() override {
		return "ColourTargetAI";
	}
private:
    std::vector<double> generatePickingWeightsFromBoard();
	std::vector<double> generatePickingWeightsFromBoardImpl(std::vector<double>* currentWeights);
	double evaluatePlacingChoice(PlacingChoice& choice, TileType bonusCol);
    double evaluatePickingChoice(PickingChoice choice, TileType bonusCol);
    std::vector<double> m_tile_picking_weights;
    TileType m_target_colour;
};

#endif