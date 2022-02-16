#include "game_elements/Button.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/String.hpp>

Button::Button(std::string str, sf::Vector2f location): m_current(false) {
    if (!m_font.loadFromFile("resources/NotoSansCJK-Medium.ttc")) {
        g_logger.log(Logger::ERROR, "Font not loaded");
    }
    m_string = sf::Text();
    m_string.setString(str);
    m_string.setPosition(location.x+3,location.y+3);
    m_string.setCharacterSize(14);
    m_string.setFont(m_font);
    // m_string.setFillColor(sf::Color::Black);
    m_outline = sf::RectangleShape(sf::Vector2f(100, 100));
    m_outline.setPosition(location);
    m_outline.setFillColor(sf::Color::Red);
}

// Button::Button(sf::Texture* normal, sf::Texture* clicked, std::string words,sf::Vector2f location) : m_normal(), m_clicked(), m_current(false) {
//     m_normal.setTexture(*normal);
//     m_clicked.setTexture(*clicked);
//     m_currentSpr =& m_normal;
//     m_normal.setPosition(location);
//     m_clicked.setPosition(location);
//     m_string.setString(words);
//     m_string.setPosition(location.x+3,location.y+3);
//     m_string.setCharacterSize(14);
// }

void Button::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    // target.draw(*m_currentSpr);
    target.draw(m_outline);
    target.draw(m_string);
}

bool Button::onClick(int x, int y) {
    sf::FloatRect rect = m_outline.getGlobalBounds();
    if (rect.contains(x,y)) {
        setState(!m_current);
        return true;
    }
    return false;
}

void Button::setState(bool which) {
    m_current = which;
    if (m_current) {
        g_logger.log(Logger::INFO, "True\n");
    } else {
        g_logger.log(Logger::INFO, "False\n");
    }
    // if (m_current) {
    //     m_currentSpr = &m_clicked;
    //     return;
    // }
    // m_currentSpr = &m_normal;
}

void Button::setText(std::string words) {
    m_string.setString(words);
}

bool Button::getVar() {
    return m_current;
}

// sf::Sprite* Button::getSprite() {
//     return m_currentSpr;
// }
