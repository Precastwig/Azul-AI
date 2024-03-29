#include "players/ColourTargetAI.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <memory>
#include <thread>
#include <utility>
#include <vector>
#include <set>

ColourTargetAI::ColourTargetAI(PlayerColour colour, std::shared_ptr<Bag> bag, std::unique_ptr<Board> board, TileType target) : Player(colour, bag, std::move(board)), m_target_colour(target) {
    // Need to apply weights to the different colours
    // This will change as ones board fills up
    m_tile_picking_weights = generatePickingWeightsFromBoard();
}

struct TileAndCount {
    TileType m_type;
    int m_count;
};

std::vector<double> ColourTargetAI::generatePickingWeightsFromBoardImpl() {
    std::vector<double> weights = {1,1,1,1,1,1};
    weights[m_target_colour] = 1.5;
    std::vector<TileType> adjacentTiles = Board::getAdjacentStarColours(m_target_colour);
     for (TileType adjacentTile : adjacentTiles) {
        weights[adjacentTile] = 1.25;
    }
    // We then modify the weights from the defaults based upon:
    // How many we have in reserve (numheld)
    // How many we want to fill (sumofcost)
    // it's contribution to getting bonus tiles (smallestNumTilesNeededForBonus)
    // Future idea: We should simulate our board using our currently held pieces, 
    // and evaluate the picking weights from the best ones,
    std::vector<int> numWanted = {0,0,0,0,0,0};
    for (TileType tilecol : Tile::all_tile_types()) {
        int sumOfCost = 0;
        std::vector<PlacingChoice> choices = m_board->getPlacingChoicesOfCol(tilecol);
        for (PlacingChoice c : choices) {
            if (tilecol == m_target_colour || c.cost.num_colour + c.cost.num_bonus <= 4) {
                sumOfCost  += c.cost.num_colour;
            }
        }        

        numWanted[tilecol] = sumOfCost - howManyColourStored(tilecol, getTiles());
    }

    // Rank the number wanted of colours, 
    auto comparator = [](TileAndCount a, TileAndCount b) {
        return a.m_count < b.m_count;
    };
    std::multiset<TileAndCount, decltype(comparator)> sortedNumWanted;
    for (TileType tilecol : Tile::all_tile_types()) {
        TileAndCount tac;
        tac.m_type = tilecol;
        tac.m_count = numWanted[tilecol];
        sortedNumWanted.insert(tac);
    }
    // and give bonus based on position
    double bonus = 0.05;
    std::cout << "Sorted num wanted\n";
    for (TileAndCount tac : sortedNumWanted) {
        std::cout << Tile::toString(tac.m_type) << ": " << tac.m_count << "\n";
        weights[tac.m_type] += bonus;
        bonus += 0.05;
    }
    return weights;
}

std::vector<double> ColourTargetAI::modifyPickingWeightsBasedOnBonus(std::vector<double> currentWeights) {
    std::vector<double> closenessToColMultiplier =    {0,0,0,0,0,0};
    for (TileType tilecol : Tile::all_tile_types()) {
        int smallestNumTilesNeededForBonus = -1;
        // Check 2/1 of tilcol and 3/4 of next colour
        int tilesNeededForStatue = m_board->tilesNeededToGetStatue(Location::location_from_col(tilecol));
        // Check 5/6 of tilecol
        int tilesNeededForWindow = m_board->tilesNeededToGetWindow(Location::location_from_col(tilecol)) - 1;
        // -1 for the extra bonus that the window gives

        // Ignore columns, for now, it's not worth it? Is it?
        if (tilesNeededForStatue > 0 && tilesNeededForWindow > 0) {
            smallestNumTilesNeededForBonus = std::min(tilesNeededForStatue, tilesNeededForWindow);
        } else if (tilesNeededForWindow > 0) {
            smallestNumTilesNeededForBonus = tilesNeededForWindow;    
        } else if (tilesNeededForStatue > 0) {
            smallestNumTilesNeededForBonus = tilesNeededForStatue;    
        } 

        // Convert the smallestNumTilesNeededForBonus into to a multiplying variable
        if (smallestNumTilesNeededForBonus > 0) {
            closenessToColMultiplier[tilecol] = std::max(0.0, 1.0 - (smallestNumTilesNeededForBonus / 10.0));
        } else {
            closenessToColMultiplier[tilecol] = 0.0;
        }
    }

    // We then modify this by the quality of the bonus tiles available
    // according to the currentWeights
    double AmountWeWantBonus = 0.0;
    for (TileType tilecol : Tile::all_tile_types()) {
        int rewardTileCount = howManyColourStored(tilecol, m_bag->rewardTiles());
        // The more reward tiles that we can use for our needs the better
        AmountWeWantBonus += rewardTileCount * currentWeights[tilecol];
        // Is this silly? Does this need to be capped at some value?
    }
        

    //smallestNumTilesNeededForBonus matters as much as there are bonus tiles we want

    // currentWeights + (inverted)smallestNumTilesNeededForBonus (smaller is better) * Amount we want the bonus
    //                      smallestNumTilesNeededForBonus == 1 ? 1 : == 7 ? 0.1
    for (TileType tilecol : Tile::all_tile_types()) {
        currentWeights[tilecol] = currentWeights[tilecol] + (closenessToColMultiplier[tilecol] * AmountWeWantBonus);
    }
    return currentWeights;
}

std::vector<double> ColourTargetAI::generatePickingWeightsFromBoard() {
    std::vector<double> weights = generatePickingWeightsFromBoardImpl();
    return modifyPickingWeightsBasedOnBonus(weights);
}

double ColourTargetAI::evaluatePickingChoice(PickingChoice choice, TileType bonusCol) {
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
    m_tile_picking_weights = generatePickingWeightsFromBoard();
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

double ColourTargetAI::evaluatePlacingChoice(PlacingChoice& choice, TileType bonusCol) {
    // Evaluate colour, higher ones are better?
    double score = choice.points_gained(); 
    score += (choice.cost.num_colour + choice.cost.num_bonus);
    if (choice.star->colour() != LocationType::CENTRE_STAR) {
        score *= m_tile_picking_weights[choice.cost.colour];
    }
    return score;
}

PlacingChoice ColourTargetAI::placeTile(Tile bonus) {
    // For now, same as picking
    m_tile_picking_weights = generatePickingWeightsFromBoard();
	std::vector<PlacingChoice> choices = getAllowedPlacingChoices(bonus);
	if (choices.size() == 0) {
		m_done_placing = true;
		return PlacingChoice();
	}

    // We need to evaluate each one and compare evaluations
    std::vector<std::pair<PlacingChoice, double>> choices_with_scores;
    for (PlacingChoice& c : choices) {
        choices_with_scores.push_back(std::pair<PlacingChoice, double>(c, evaluatePlacingChoice(c, bonus.colour())));
    }
    std::sort(choices_with_scores.begin(), choices_with_scores.end(), [&](std::pair<PlacingChoice, double>& c1, std::pair<PlacingChoice, double>& c2){
        return c1.second > c2.second;
    });
    if (numTiles() > 4 || bonus == TileType::RED) {
        // We might as well place something...?
        return choices_with_scores[0].first;
    }
    if (choices_with_scores[0].second <= 1.0) {
        // Below 1 isn't even worth it...?
        m_done_placing = true;
        return PlacingChoice();
    }
    return choices[0];
}

std::vector<std::shared_ptr<Tile>> ColourTargetAI::chooseBonusPieces(std::vector<std::shared_ptr<Tile>> choices) {
    std::vector<std::shared_ptr<Tile>> retlist;
    for (std::shared_ptr<Tile> choice : choices) {
        if (m_bonus_to_choose > 0) {
            retlist.push_back(choice);
            m_bonus_to_choose--;
        } else {
            break;
        }
    }
    return retlist;
}

void ColourTargetAI::discardDownToFour() {
    while (getTiles().size() > 4) {
        discardTile(getTiles()[0]);
    }
}
