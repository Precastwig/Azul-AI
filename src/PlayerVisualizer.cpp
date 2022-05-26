#include "game_elements/PlayerVisualizer.hpp"
#include "players/Player.hpp"
#include "ui_elements/Button.hpp"
#include "utils/helper_enums.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <string>
#include <vector>
#include <players/PlayerInfo.hpp>

sf::Color bgCol = sf::Color(255, 228,204);
sf::Color lineCol = sf::Color(255, 190, 134);
sf::Color txtCol = sf::Color(sf::Color::Black);
extern PlayerInfo g_player_info;

PlayerVisualizer::PlayerVisualizer(std::shared_ptr<Player> player, sf::Vector2f location, sf::Vector2f size) : m_player(player), m_first_tile() {
    m_outline = sf::RectangleShape(size);
    m_outline.setPosition(location);
    m_outline.setFillColor(bgCol);
    m_outline.setOutlineColor(lineCol);
    m_outline.setOutlineThickness(1.0);
    // Place the title top centre
    m_player_name = sf::Text();
    m_player_name.setPosition(sf::Vector2f(location.x + size.x/20.0f,location.y + size.y/20.0f));
    m_player_name.setFont(g_font);
    m_player_name.setCharacterSize(15);
    m_player_name.setColor(txtCol);
    updateString();
    // Place the button at the bottom?
    sf::Vector2f buttonsize = sf::Vector2f(size.x, size.y / 10);
    m_doneplacing_button.setSize(buttonsize);
    m_doneplacing_button.setPosition(sf::Vector2f(location.x, location.y + (size.y - buttonsize.y)));
    m_doneplacing_button.setText("Done placing");
    m_doneplacing_button.m_callback = std::bind(&PlayerVisualizer::donePlacing, this);

    sf::Vector2f first_pos(
        location.x + size.x - (m_first_tile.getSize().x * 2),
        location.y + (m_first_tile.getSize().y * 2)
    );
    m_first_tile.setPosition(first_pos);
}

void PlayerVisualizer::donePlacing() {
    if (m_player->getTiles().size() <= 4) {
        g_visual_state.set_placing();
        m_doneplacing_button.setText("Done placing"); // Reset button text
        m_player->pass();
        return;
    }
    if (g_visual_state.is_placing()) {
        // We need to show something to select which tiles to lose
        g_visual_state.set_discarding();
        m_doneplacing_button.setText("Done Discarding");
    } else if (g_visual_state.is_discarding()) {
        // Do a warning flash thing? or something
        g_logger.log(Logger::WARNING, "Pressed button when had more than 4 tiles left");
    }
}

void PlayerVisualizer::updatePlayerTilePositions() {
    // Move the tiles to a good spot
    std::vector<std::shared_ptr<Tile>> tiles = m_player->getTiles();
    if (tiles.empty()) {
        return;
    }
    sf::Vector2f tileSize = tiles[0]->getSize();
    sf::Vector2f bgPos = m_outline.getPosition();
    sf::Vector2f bgSize = m_outline.getSize();
    float startx = bgPos.x + tileSize.x;
    float starty = m_player_name.getGlobalBounds().top + m_player_name.getGlobalBounds().height + (tileSize.y * 2);
    float endx = bgPos.x + bgSize.x * 8 / 10;
    float xincrement = tileSize.x;
    float yincrement = tileSize.y * 2;
    float currX = startx;
    float currY = starty;
    for (std::shared_ptr<Tile> tile : m_player->getTiles()) {
        tile->setPosition(sf::Vector2f(
            currX,
            currY
        ));
        tile->setRotation(0.0);
        tile->setOutlineThickness(0.0);
        if (currX < endx) {
            currX += xincrement;
        } else {
            currX = startx;
            currY += yincrement;
        }
    }
}

void PlayerVisualizer::onClick(int x, int y) {
    if (g_player_info.getCurrentPlayer() != m_player) {
        g_logger.log(Logger::INFO, "Not this players turn");
        return;
    }
    if (g_visual_state.is_discard_button_clickable()) {
        m_doneplacing_button.onClick(x, y);
    }
    if (g_visual_state.is_discarding()) {
        // Reverse through the list, as they were visualised the other way!
        std::vector<std::shared_ptr<Tile>> tiles = m_player->getTiles();
        for (std::vector<std::shared_ptr<Tile>>::reverse_iterator it = tiles.rbegin(); 
        it != tiles.rend(); ++it ) { 
            std::shared_ptr<Tile> tile = *it;
            if (tile->contains(x, y)) {
                Sounds::crumple();
                m_player->discardTile(tile);
                return;
            }
        } 
    }
}

bool PlayerVisualizer::contains(int x, int y) {
    return m_outline.getGlobalBounds().contains(x, y);
}

std::shared_ptr<Player> PlayerVisualizer::getPlayerVisualized() {
    return m_player;
}

void PlayerVisualizer::onHover(int x, int y) {
    if (g_player_info.getCurrentPlayer() != m_player) {
        return;
    }
    if (g_visual_state.is_placing()) {
        m_doneplacing_button.onHover(x, y);
    }

    if (g_visual_state.is_discarding() &&
        g_player_info.getCurrentPlayer() == m_player) {
        // Reverse through the list for the hovering
        std::vector<std::shared_ptr<Tile>> tiles = m_player->getTiles();
        bool outlined = false;
        for (std::vector<std::shared_ptr<Tile>>::reverse_iterator it = tiles.rbegin(); 
        it != tiles.rend(); ++it ) { 
            std::shared_ptr<Tile> tile = *it;
            if (!outlined && tile->contains(x, y)) {
                tile->setOutlineThickness(2.0);
                outlined = true;
            } else {
                tile->setOutlineThickness(0.0);
            }
        } 
    }
}

void PlayerVisualizer::update() {
    updateString();
    updatePlayerTilePositions();
    if (m_player == g_player_info.getCurrentPlayer()) {
        m_outline.setOutlineColor(sf::Color::Black);
        m_outline.setOutlineThickness(2.5);
    } else {
        m_outline.setOutlineColor(lineCol);
        m_outline.setOutlineThickness(1.0);
    }
}

void PlayerVisualizer::updateString() {
    m_player_name.setString(m_player->colourString() + "\n" + m_player->playerTypeString() + "\nPoints: " + std::to_string(m_player->points()));
}

void PlayerVisualizer::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(m_outline, states);
    target.draw(m_player_name, states);
    if (g_visual_state.is_discard_button_clickable()) {
        target.draw(m_doneplacing_button, states);
    } 
    if (g_player_info.getStartingPlayer() == m_player && g_player_info.centreTaken()) {
        target.draw(m_first_tile, states);
    }
    for (std::shared_ptr<Tile> tile : m_player->getTiles()) {
        target.draw(*tile, states);
    }
    if (g_player_info.getCurrentPlayer() != m_player) {
        return;
    }
}