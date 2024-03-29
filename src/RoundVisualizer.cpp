#include "utils/Sounds.hpp"
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <game_elements/RoundVisualizer.hpp>
#include <string>
#include <thread>

extern sf::Font g_font;

RoundVisualizer::RoundVisualizer(std::vector<TileType> order, sf::Vector2f position) : m_current_round(0) {
    int spreadval = 150;
    position = position - sf::Vector2f(2.5 * spreadval, 0);
    for (unsigned int i = 0; i < order.size(); ++i) {
        m_bonusmarkers.push_back(Tile(order[i]));
        m_bonusmarkers[i].setPosition(position);
        sf::Text txt;
        txt.setFont(g_font);
        txt.setCharacterSize(14);
        txt.setString(std::to_string(i + 1));
        sf::FloatRect playerRect = txt.getLocalBounds();
        txt.setOrigin(playerRect.left + playerRect.width/2.0f,
                        playerRect.top  + playerRect.height/2.0f);
        txt.setPosition(position);
        m_roundmarkers.push_back(txt);
        position = position + sf::Vector2f(spreadval, 0);
    }
    updateVisuals();
}

void RoundVisualizer::nextround() {
    m_current_round++;
    Sounds::dong();
    std::thread spinnerThread(&RoundVisualizer::spinTile, std::ref(*this));
    spinnerThread.detach();
    updateVisuals();
}

void RoundVisualizer::spinTile() {
    // 3 seconds, spin 6 times  
    // 720 degrees/sec
    float spin_velocity = 720; // Maybe?
    float curr_spin = 0;
    float total_spin = 720;
    sf::Clock clock;
    while (curr_spin < total_spin) {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        float spinby = spin_velocity * time;
        m_bonusmarkers[m_current_round].rotate(spinby);
        curr_spin += spinby;
    }
}

void RoundVisualizer::updateVisuals() {
    for (size_t i = 0; i < m_bonusmarkers.size(); ++i) {
        if (i == m_current_round) {
            m_bonusmarkers[i].setOutlineThickness(4.0);
        } else {
            m_bonusmarkers[i].setOutlineThickness(0.0);
        }
    }
}


void RoundVisualizer::draw (sf::RenderTarget &target, sf::RenderStates states) const {
    for (size_t i = 0; i < m_bonusmarkers.size(); ++i) {
        target.draw(m_bonusmarkers[i], states);
        target.draw(m_roundmarkers[i], states);
    }
}