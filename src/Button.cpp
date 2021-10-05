#include "game_elements/Button.hpp"

Button::Button(sf::Texture* normal, sf::Texture* clicked, std::string words,sf::Vector2f location) : m_current(false) {
    m_normal.setTexture(*normal);
    m_clicked.setTexture(*clicked);
    m_currentSpr =& m_normal;
    m_normal.setPosition(location);
    m_clicked.setPosition(location);
    m_string.setString(words);
    m_string.setPosition(location.x+3,location.y+3);
    m_string.setCharacterSize(14);
}

bool Button::onClick(int x, int y) {
    sf::FloatRect rect = m_currentSpr->getGlobalBounds();
    if (rect.contains(x,y)) {
        setState(!m_current);
        return true;
    }
    return false;
}

void Button::setState(bool which) {
    m_current = which;
    if (m_current) {
        m_currentSpr = &m_clicked;
        return;
    }
    m_currentSpr = &m_normal;
}

void Button::setText(std::string words) {
    m_string.setString(words);
}

bool Button::getVar() {
    return m_current;
}

sf::Sprite* Button::getSprite() {
    return m_currentSpr;
}
