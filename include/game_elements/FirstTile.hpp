#ifndef FIRSTTILE
#define FIRSTTILE

#include "game_elements/Tile.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Window/Keyboard.hpp>

extern sf::Font g_font;

class FirstTile : public sf::Drawable {
public:
    FirstTile() : m_tile(TileType::NONE, Vector2f(25, 15)) {
        m_tile.setFillColor(sf::Color::White);
        m_tile.setOutlineColor(sf::Color::Red);
        setHovered(false);
        m_one.setFont(g_font);
        m_one.setCharacterSize(25);
        m_one.setString("1");
        m_one.setColor(sf::Color::Black);
        sf::FloatRect rect = m_one.getLocalBounds();
        m_one.setOrigin(sf::Vector2f(rect.left + rect.width/2.0,rect.top + rect.height/2.0));
        m_decoration.setRadius(std::max(rect.height, rect.width));
        m_decoration.setFillColor(sf::Color(0,0,0,0));
        m_decoration.setOutlineColor(sf::Color(0,0,0,255));
        m_decoration.setOutlineThickness(1);
        sf::FloatRect rect2 = m_decoration.getLocalBounds();
        m_decoration.setOrigin(sf::Vector2f(rect2.left + rect2.width/2.0,rect2.top + rect2.height/2.0));
        setRotation(0);
    }
    bool contains(int x, int y) {
        return m_tile.contains(x, y);
    }
    void setHovered(bool state) {
        if (state) {
            m_tile.setOutlineThickness(1.0);
        } else {
            m_tile.setOutlineThickness(0.0);
        }
    }
    void setPosition(sf::Vector2f pos) {
        m_tile.setPosition(pos);
        m_one.setPosition(pos);
        m_decoration.setPosition(pos);
    }
    void setRotation(double r) {
        m_tile.setRotation(r);
        m_one.setRotation(r);
    }
    sf::Vector2f getSize() {
        return m_tile.getSize();
    }
    virtual void draw (RenderTarget &target, RenderStates states) const override {
        // First draw the tile
        target.draw(m_tile, states);
        // Then the text
        target.draw(m_one, states);
        target.draw(m_decoration, states);
    }
private:
    Tile m_tile;
    sf::Text m_one;
    sf::CircleShape m_decoration;
};

#endif