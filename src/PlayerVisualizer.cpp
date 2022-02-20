#include "game_elements/PlayerVisualizer.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <memory>

PlayerVisualizer::PlayerVisualizer(std::shared_ptr<Player> player, sf::Vector2f location, sf::Vector2f size) : m_player(player){
    if (!m_font.loadFromFile("resources/NotoSansCJK-Medium.ttc")) {
        g_logger.log(Logger::ERROR, "Font not loaded");
    }
    m_outline = sf::RectangleShape(size);
    m_outline.setPosition(location);
    m_outline.setFillColor(sf::Color(255, 228,204));
    m_outline.setOutlineColor(sf::Color(255, 190, 134));
    // Place the title top centre
    sf::FloatRect playerRect = m_player_name.getLocalBounds();
    m_player_name = sf::Text();
    m_player_name.setOrigin(playerRect.left + playerRect.width/2.0f,
                            playerRect.top  + playerRect.height/2.0f);
    m_player_name.setPosition(sf::Vector2f(size.x/2.0f,size.y/2.0f));
    m_player_name.setString(player->toStringNoBoard());
    m_player_name.setFont(m_font);
    m_player_name.setCharacterSize(20);
    m_player_name.setColor(sf::Color::Black);
}

void PlayerVisualizer::updateString() {
    m_player_name.setString(m_player->toStringNoBoard());
}

void PlayerVisualizer::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(m_player_name);
    target.draw(m_outline);
}