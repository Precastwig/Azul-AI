#include "game_elements/PlayerVisualizer.hpp"
#include "players/Player.hpp"
#include "utils/helper_enums.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <vector>

sf::Color bgCol = sf::Color(255, 228,204);
sf::Color lineCol = sf::Color(255, 190, 134);
sf::Color txtCol = sf::Color(sf::Color::Black);

PlayerVisualizer::PlayerVisualizer(std::shared_ptr<Player> player, sf::Vector2f location, sf::Vector2f size) : m_player(player) {
    if (!m_font.loadFromFile("resources/NotoSansCJK-Medium.ttc")) {
        g_logger.log(Logger::ERROR, "Font not loaded");
    }
    m_outline = sf::RectangleShape(size);
    m_outline.setPosition(location);
    m_outline.setFillColor(bgCol);
    m_outline.setOutlineColor(lineCol);
    m_outline.setOutlineThickness(1.0);
    // Place the title top centre
    m_player_name = sf::Text();
    m_player_name.setString(player->toStringNoBoard());
    m_player_name.setPosition(sf::Vector2f(location.x + size.x/10.0f,location.y + size.y/10.0f));
    m_player_name.setFont(m_font);
    m_player_name.setCharacterSize(20);
    m_player_name.setColor(txtCol);
    // Place the button at the bottom?
    sf::Vector2f buttonsize = sf::Vector2f(size.x, size.y / 5);
    m_doneplacing_button.setSize(buttonsize);
    m_doneplacing_button.setPosition(sf::Vector2f(location.x, location.y + (size.y - buttonsize.y)));
    m_doneplacing_button.setText("Done placing");
    m_doneplacing_button.m_callback = std::bind(&PlayerVisualizer::donePlacing, this);
}

void PlayerVisualizer::donePlacing() {
    // We need to show something to select which tiles to lose
    if (g_visual_state == GameState::PLACING) {
        g_visual_state = GameState::DISCARDING;
        m_doneplacing_button.setText("Done Discarding");
        // Move the tiles to a good spot, here I guess?
        sf::Vector2f bgPos = m_outline.getPosition();
        sf::Vector2f bgSize = m_outline.getSize();
        float startx = bgPos.x + bgSize.x * 2 / 10;
        float endx = bgPos.x + bgSize.x * 8 / 10;
        float xincrement = bgSize.x / 10;
        float currX = startx;
        float starty = bgPos.y + (bgSize.y / 2);
        float yincrement = bgSize.y / 10;
        float currY = starty;
        for (std::shared_ptr<Tile> tile : m_player->getTiles()) {
            tile->setPosition(sf::Vector2f(
                currX,
                currY
            ));
            tile->setRotation(0.0); // Might have to be 90 deg
            if (currX < endx) {
                currX += xincrement;
            } else {
                currX = startx;
                currY += yincrement;
            }
        }
    } else if (g_visual_state == GameState::DISCARDING) {
        if (m_player->getTiles().size() <= 4) {
            m_player->pass();
        } else {
            // Do a warning flash thing? or something
            g_logger.log(Logger::WARNING, "Pressed button when had more than 4 tiles left");
        }
    }
    // Display the tiles and let user click them in game.cpp
    // we dont want to pass until we are ready, should be done by clicking this
}

void PlayerVisualizer::onClick(int x, int y) {
    if (g_visual_state == GameState::PLACING || g_visual_state == GameState::DISCARDING) {
        m_doneplacing_button.onClick(x, y);
    }
    if (g_visual_state == GameState::DISCARDING) {
        // Reverse through the list, as they were visualised the other way!
        std::vector<std::shared_ptr<Tile>> tiles = m_player->getTiles();
        for (std::vector<std::shared_ptr<Tile>>::reverse_iterator it = tiles.rbegin(); 
        it != tiles.rend(); ++it ) { 
            std::shared_ptr<Tile> tile = *it;
            if (tile->contains(x, y)) {
                m_player->discardTile(tile);
                return;
            }
        } 
    }
}

void PlayerVisualizer::onHover(int x, int y) {
    if (g_visual_state == GameState::PLACING) {
        if (m_doneplacing_button.contains(x, y)) {
            if (!m_doneplacing_button.getState()) {
                m_doneplacing_button.setHoverState(true);
            }
        } else {
            if (m_doneplacing_button.getState()) {
                m_doneplacing_button.setHoverState(false);
            }
        }
    }

    if (g_visual_state == GameState::DISCARDING) {
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

void PlayerVisualizer::update(std::shared_ptr<Player> currentPlayer) {
    updateString();
    if (m_player == currentPlayer) {
        m_outline.setOutlineColor(sf::Color::Black);
        m_outline.setOutlineThickness(2.5);
    } else {
        m_outline.setOutlineColor(lineCol);
        m_outline.setOutlineThickness(1.0);
    }
    if (currentPlayer->finishedPlacing()) {
        m_doneplacing_button.setOutline(false);
        m_doneplacing_button.setOnState(true);
    } else {
        m_doneplacing_button.setOnState(false);
    }
}

void PlayerVisualizer::updateString() {
    m_player_name.setString(m_player->toStringNoBoard());
}

void PlayerVisualizer::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(m_outline, states);
    target.draw(m_player_name, states);
    if (g_visual_state == GameState::PLACING || g_visual_state == GameState::DISCARDING) {
        target.draw(m_doneplacing_button, states);
    } 

    if (g_visual_state == GameState::DISCARDING) {
        for (std::shared_ptr<Tile> tile : m_player->getTiles()) {
            target.draw(*tile, states);
        }
    }
}