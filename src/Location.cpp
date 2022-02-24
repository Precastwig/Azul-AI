#include "utils/Choices.hpp"
#include "utils/helper_enums.hpp"
#include <game_elements/Location.hpp>
#include <Game.hpp>
#include <memory>

Location::Location(LocationType l) : m_l(l), m_starting_rotation(0.0), m_current_pos() {
    for (size_t j = 0; j < 6; ++j) {
        m_visual_tiles.push_back(std::make_shared<BoardTile>(j, get_tile_type()));
    }
    updateTilesPos();
};

void Location::draw (sf::RenderTarget &target, sf::RenderStates states) const  {
    for (auto tile : m_visual_tiles) {
        tile->draw(target, states);
    }
}

void Location::onClick(int x, int y, Game& game) {
    for (auto tile : m_visual_tiles) {
        if (tile->contains(x,y) && !tile->isFilled()) {
            g_logger.log(Logger::INFO, "Clicked on tile");
            std::vector<PlacingChoice> choices = game.getCurrentPlayer()->getAllowedPlacingChoices(game.getBonus());
            choices = PlacingChoice::filterChoicesFromLocation(choices, colour());
            for (PlacingChoice choice : choices) {
                if (choice.index.getIndex() == tile->getIndex()) {
                    if (m_l == LocationType::CENTRE_STAR) {
                        // We need to set the tile to the correct colour
                        tile->setTileCol(choice.cost.colour);
                    }
                    game.place_tile(choice); 
                    return;
                }
            }
        }
    }
}

void Location::onHover(int xpos, int ypos) {
    for (std::shared_ptr<BoardTile> t : m_visual_tiles) {
        if (t->contains(xpos, ypos)) {
            if (!t->getHoverState()) {
                t->setHoverState(true);
            }
        } else {
            if (t->getHoverState()) {
                t->setHoverState(false);
            }
        }
    }
}

void Location::updateTilesPos() {
    double angle = m_starting_rotation;
    for (size_t i = 0; i < m_visual_tiles.size(); ++i) {
        sf::Vector2f tilePos = Factory::calculateNewPos(m_current_pos, 55, angle);
        m_visual_tiles[i]->setPosition(tilePos);
        m_visual_tiles[i]->setRotation(angle * 180 / M_PI);
        angle += (2 * M_PI / m_visual_tiles.size());
    }
}