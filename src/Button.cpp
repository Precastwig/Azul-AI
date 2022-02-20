#include "game_elements/Button.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/String.hpp>

Button::Button(std::string str, sf::Vector2f location): m_current(false), m_outline_on(true) {
    if (!m_font.loadFromFile("resources/NotoSansCJK-Medium.ttc")) {
        g_logger.log(Logger::ERROR, "Font not loaded");
    }
    m_string = sf::Text();
    m_string.setString(str);
    m_string.setCharacterSize(14);
    m_string.setFont(m_font);
    sf::FloatRect playerRect = m_string.getLocalBounds();
    m_string.setOrigin(playerRect.left + playerRect.width/2.0f,
                        playerRect.top  + playerRect.height/2.0f);
    // m_string.setFillColor(sf::Color::Black);
    m_outline = sf::RectangleShape(sf::Vector2f(100, 100));
    m_outline.setFillColor(sf::Color::Red);
    
    setPosition(location);

    m_callback = [](){};
}

void Button::setSize(sf::Vector2f size) {
    m_outline.setSize(size);
    setPosition(m_outline.getPosition());
}

void Button::setPosition(sf::Vector2f newpos) {
    m_outline.setPosition(newpos);
    float stringx = newpos.x + m_outline.getSize().x/2.0f;
    float stringy = newpos.y + m_outline.getSize().y/2.0f;
    m_string.setPosition(sf::Vector2f(stringx,stringy));
}

void Button::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    // target.draw(*m_currentSpr);
    if (m_outline_on) {
        target.draw(m_outline);
    }
    target.draw(m_string);
}

bool Button::onClick(int x, int y) {
    sf::FloatRect rect = m_outline.getGlobalBounds();
    if (rect.contains(x,y)) {
        m_callback();
        //setState(!m_current);
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

// sf::Sprite* Button::getSprite() {
//     return m_currentSpr;
// }
