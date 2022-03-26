#include "utils/Choices.hpp"
#include "utils/helper_enums.hpp"
#include <asm-generic/errno.h>
#include <game_elements/Location.hpp>
#include <Game.hpp>
#include <memory>
#include <players/PlayerInfo.hpp>

extern PlayerInfo g_player_info;

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
            PlacingChoice pc = get_choice();
            // Sanity check
            if (pc.index == tile->getIndex() - 1) {
                game.place_tile(pc);
                return;
            } else {
                g_logger.log(Logger::ERROR, "Highlighted choice is not relevent to clicked tile (choice: " + pc.to_string() + ")");
            }
        }
    }
}

void Location::onHover(int xpos, int ypos, Game& game) {
    bool no_tiles_clicked = true;
    for (std::shared_ptr<BoardTile> t : m_visual_tiles) {
        if (t->isFilled()) {
            continue;
        }
        if (t->contains(xpos, ypos)) {
            if (!t->getHoverState()) {
                std::vector<PlacingChoice> choices = g_player_info.getCurrentPlayer()->getAllowedPlacingChoices(game.getBonus());
                choices = PlacingChoice::filterChoicesFromLocation(choices, colour());
                choices = PlacingChoice::filterChoicesFromIndex(choices, t->getIndex());
                set_choices(choices, game.getBonus());
                t->setHoverState(true);
            }
        } else {
            if (t->getHoverState()) {
                t->setHoverState(false);
                wipe_choices(game.getBonus());
            }
        }
    }
}

void Location::onLeft(TileType bonus) {
    if (m_curr_choices.empty()) {
        return;
    }
    m_index--;
    update_highlight(bonus);
}

void Location::onRight(TileType bonus) {
    if (m_curr_choices.empty()) {
        return;
    }
    m_index++;
    update_highlight(bonus);
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

PlacingChoice Location::get_choice() {
    if (m_curr_choices.empty()) {
        return PlacingChoice();
    }
    return m_curr_choices[m_index.getIndex()];
}

void Location::set_choices(std::vector<PlacingChoice> c, TileType bonus) {
    m_curr_choices = c;
    m_index = cIndex(1, m_curr_choices.size());
    update_highlight(bonus);
}

void Location::update_highlight(TileType bonus) {
    std::shared_ptr<Player> curr_player = g_player_info.getCurrentPlayer();
    curr_player->highlightCostTiles(get_choice(), bonus);
}