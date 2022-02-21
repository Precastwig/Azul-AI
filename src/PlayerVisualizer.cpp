#include "game_elements/PlayerVisualizer.hpp"
#include "players/Player.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <memory>

sf::Color bgCol = sf::Color(255, 228,204);
sf::Color lineCol = sf::Color(255, 190, 134);
sf::Color txtCol = sf::Color(sf::Color::Black);

PlayerVisualizer::PlayerVisualizer(std::shared_ptr<Player> player, sf::Vector2f location, sf::Vector2f size) : m_player(player){
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
}

void PlayerVisualizer::updateString() {
    m_player_name.setString(m_player->toStringNoBoard());
}

void PlayerVisualizer::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(m_outline);
    target.draw(m_player_name);
}