#include "players/ColourTargetAI.hpp"
#include <cstddef>
#include <memory>
#include <vector>
#include <set>

ColourTargetAI::ColourTargetAI(PlayerColour colour, std::shared_ptr<Bag> bag, Tile::Type target) : Player(colour, bag), m_target_colour(target) {
    // Need to apply weights to the different colours
    // This will change as ones board fills up
    m_tile_picking_weights = generatePickingWeightsFromBoard();
}

struct TileAndCount {
    Tile::Type m_type;
    int m_count;
};

std::vector<double> ColourTargetAI::generatePickingWeightsFromBoardImpl(std::vector<double>* currentWeights) {
    std::vector<double> weights = {1,1,1,1,1,1};
    weights[m_target_colour] = 1.5;
    std::vector<Tile::Type> adjacentTiles = Board::getAdjacentStarColours(m_target_colour);
     for (Tile::Type adjacentTile : adjacentTiles) {
        weights[adjacentTile] = 1.25;
    }
    // We then modify the weights from the defaults based upon:
    // How many we have in reserve (numheld)
    // How many we want to fill (sumofcost)
    // it's contribution to getting bonus tiles (smallestNumTilesNeededForBonus)
    // Future idea: We should simulate our board using our currently held pieces, 
    // and evaluate the picking weights from the best ones,
    std::vector<int> smallestNumTilesNeededForBonus = {0,0,0,0,0,0};
    std::vector<int> numWanted = {0,0,0,0,0,0};
    for (Tile::Type tilecol : Tile::all_tile_types()) {
        int sumOfCost = 0;
        std::vector<PlacingChoice> choices = m_board.getPlacingChoicesOfCol(tilecol);
        for (PlacingChoice c : choices) {
            if (tilecol == m_target_colour || c.cost.num_colour + c.cost.num_bonus <= 4) {
                sumOfCost  += c.cost.num_colour;
            }
        }        

        numWanted[tilecol] = sumOfCost - howManyColourStored(tilecol, m_stored_tiles);

        // Bonus stuff, we can only evaluate bonus info if we know the value of all the tile colours
        if (currentWeights) {  
            // Check 2/1 of tilcol and 3/4 of next colour
            int tilesNeededForStatue = m_board.tilesNeededToGetStatue(Location::location_from_col(tilecol));
            // Check 5/6 of tilecol
            int tilesNeededForWindow = m_board.tilesNeededToGetWindow(Location::location_from_col(tilecol)) - 1;
            // -1 for the extra bonus that the window gives

            // Ignore columns, for now, it's not worth it? Is it?
            if (tilesNeededForStatue > 0 && tilesNeededForWindow > 0) {
                smallestNumTilesNeededForBonus[tilecol] = std::min(tilesNeededForStatue, tilesNeededForWindow);
            } else if (tilesNeededForWindow > 0) {
                smallestNumTilesNeededForBonus[tilecol] = tilesNeededForWindow;    
            } else if (tilesNeededForStatue > 0) {
                smallestNumTilesNeededForBonus[tilecol] = tilesNeededForStatue;    
            } 
        }
    }

    if (currentWeights) {   
        // We then modify this by the quality of the bonus tiles available
        // according to the currentWeights
        bool rewardTilesUseful = false;
        std::vector<int> rewardTileCount = {0,0,0,0,0,0};
        for (Tile::Type tilecol : Tile::all_tile_types()) {
            int rewardTileCount = howManyColourStored(tilecol, m_bag->rewardTiles());
            // If there are enough reward tiles for our needs then this is valid
            //if (rewardTileCount > 2 && numWanted[tilecol] )
        }
        

        //smallestNumTilesNeededForBonus;

    }

    // Rank the number wanted of colours, 
    auto comparator = [](TileAndCount a, TileAndCount b) {
        return a.m_count < b.m_count;
    };
    std::set<TileAndCount, decltype(comparator)> sortedNumWanted;
    for (Tile::Type tilecol : Tile::all_tile_types()) {
        TileAndCount tac;
        tac.m_type = tilecol;
        tac.m_count = numWanted[tilecol];
        sortedNumWanted.insert(tac);
    }
    // and give bonus based on position
    double bonus = 0.05;
    for (TileAndCount tac : sortedNumWanted) {
        weights[tac.m_type] += bonus;
        bonus += 0.05;
    }
}

std::vector<double> ColourTargetAI::generatePickingWeightsFromBoard() {
    std::vector<double> weights = generatePickingWeightsFromBoardImpl(nullptr);
    return generatePickingWeightsFromBoardImpl(&weights);
}

double ColourTargetAI::evaluatePickingChoice(PickingChoice choice, Tile::Type bonusCol) {
    // More tiles == more good
    int numberOfTilesTaken = choice.factory->numberOf(choice.tile_colour);
    double bonusScore = 0;
    if (choice.with_bonus) {
        bonusScore = m_tile_picking_weights[bonusCol] + 1;
    }
    double score = numberOfTilesTaken * 2; // Exponentially weighted
    // Modify the score by the colour weighting
    score *= m_tile_picking_weights[m_target_colour];
    return score + bonusScore;
}

PickingChoice ColourTargetAI::pickTile(
	std::vector<std::shared_ptr<Factory>> factories,
	std::shared_ptr<Factory> centre,
	Tile bonus,
	bool centrePoison
) {
    generatePickingWeightsFromBoard();
	std::vector<PickingChoice> choices = getAllPickingChoices(
		factories,
		centre,
		bonus.colour()
	);
    // What parameters should we choose?
    // We should evaluate each choice and give it a score, then choose the picking
    // choice with the highest score
    // The evaluation should take into account our current board state
    double currentBestScore = 0;
    PickingChoice currentBestChoice = choices[0];
    for (PickingChoice choice : choices) {
        double score = evaluatePickingChoice(choice, bonus.colour());
        if (score > currentBestScore) {
            currentBestScore = score;
            currentBestChoice = choice;
        }
    }
    return currentBestChoice;
}

PlacingChoice ColourTargetAI::placeTile(Tile bonus) {
	std::vector<PlacingChoice> choices = getAllowedPlacingChoices(bonus);
	if (choices.size() == 0) {
		m_done_placing = true;
		return PlacingChoice();
	}
    return choices[0];
}

std::vector<std::shared_ptr<Tile>> ColourTargetAI::chooseBonusPieces(std::vector<std::shared_ptr<Tile>> choices, int number) {
    std::vector<std::shared_ptr<Tile>> retlist;
    for (std::shared_ptr<Tile> choice : choices) {
        if (number > 0) {
            retlist.push_back(choice);
            number--;
        } else {
            break;
        }
    }
    return retlist;
}

std::vector<std::shared_ptr<Tile>> ColourTargetAI::discardDownToFour() {
    return {};
}
