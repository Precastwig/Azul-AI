#include "Colours.hpp"
#include "utils/Sounds.hpp"
#include <SFML/Graphics/Color.hpp>
#include <ui_elements/Button.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/String.hpp>

extern sf::Font g_font;

Button::Button(std::string str, sf::Vector2f location) {
    m_string = sf::Text();
    m_string.setString(str);
    m_string.setCharacterSize(14);
    m_string.setFont(g_font);
    sf::FloatRect playerRect = m_string.getLocalBounds();
    m_string.setOrigin(playerRect.left + playerRect.width/2.0f,
                        playerRect.top  + playerRect.height/2.0f);
    // m_string.setFillColor(sf::Color::Black);
    m_background = sf::RectangleShape(sf::Vector2f(100, 100));
    m_background.setFillColor(Colours::button_default_col());
    m_background.setOutlineThickness(0.0);
    setPosition(location);

    m_callback = [](){};
}

void Button::setSize(sf::Vector2f size) {
    m_background.setSize(size);
    setPosition(m_background.getPosition());
}

void Button::setPosition(sf::Vector2f newpos) {
    m_background.setPosition(newpos);
    float stringx = newpos.x + m_background.getSize().x/2.0f;
    float stringy = newpos.y + m_background.getSize().y/2.0f;
    m_string.setPosition(sf::Vector2f(stringx,stringy));
}

void Button::setColour(sf::Color col) {
    m_background.setFillColor(col);
}

void Button::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    // target.draw(*m_currentSpr);
    target.draw(m_background);
    target.draw(m_string);
}

bool Button::contains(int x, int y) {
    return m_background.getGlobalBounds().contains(x,y);
}

bool Button::onClick(int x, int y) {
    sf::FloatRect rect = m_background.getGlobalBounds();
    if (rect.contains(x,y)) {
        m_callback();
        //setState(!m_current);
        return true;
    }
    return false;
}

void Button::onHover(int x, int y) {
    if (contains(x,y)) {
        if (!m_hovered) {
            Sounds::click();
            m_hovered = true;
        }
    } else {
        if (m_hovered) {
            m_hovered = false;
        }
    }
    updateColours();
}

void Button::setText(std::string words) {
    m_string.setString(words);
    sf::FloatRect playerRect = m_string.getLocalBounds();
    m_string.setOrigin(playerRect.left + playerRect.width/2.0f,
                    playerRect.top  + playerRect.height/2.0f);
    setPosition(m_background.getPosition());
}

void Button::updateColours() {
    // Can this be merged with BoardTile's hoverability?
    // Yes but I CBA
    if (m_hovered) {
        sf::Color col = m_background.getFillColor();
        col.a = 200;
        m_background.setFillColor(col);
    } else {
        sf::Color col = m_background.getFillColor();
        col.a = 125;
        m_background.setFillColor(col);
    }
}

// sf::Sprite* Button::getSprite() {
//     return m_currentSpr;
// }
