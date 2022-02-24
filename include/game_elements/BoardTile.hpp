#ifndef BOARDTILE
#define BOARDTILE

#include "utils/helper_enums.hpp"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <game_elements/Tile.hpp>

// Small wrapper for Tile that adds a number to be rendered
class BoardTile : sf::Drawable {
public:
    BoardTile(int num, TileType t): m_tile(t), m_num(num), m_filled(false), m_hovered(false) {
        if (!m_font.loadFromFile("resources/NotoSansCJK-Medium.ttc")) {
            g_logger.log(Logger::ERROR, "Font not loaded");
        }
        m_num_txt = sf::Text();
        m_num_txt.setFont(m_font);
        m_num_txt.setString(std::to_string(num + 1));
        m_num_txt.setColor(sf::Color::Black);
        m_num_txt.setCharacterSize(20);
        sf::FloatRect rect = m_num_txt.getLocalBounds();
        m_num_txt.setOrigin(sf::Vector2f(rect.width/2,rect.height/2));
        updateColours();
    }

    void setHoverState(bool hovered) {
        m_hovered = hovered;
        updateColours();
    }

    bool getHoverState() {
        return m_hovered;
    }

    void setFilled(bool filled) {
        m_filled = filled;
        updateColours();
    }

    bool isFilled() {
        return m_filled;
    }

    void setTileCol(TileType t) {
        if (m_tile.colour() != TileType::NONE) {
            // We should only be able to set NONE type to something
            g_logger.log(Logger::ERROR, "Attempting to set the type of regular tile");
            return;
        }
        m_tile.setType(t);
        updateColours();
    }

    void setPosition(sf::Vector2f pos) {
        m_tile.setPosition(pos);
        m_num_txt.setPosition(pos);
    }

    void setRotation(double r) {
        m_tile.setRotation(r);
        m_num_txt.setRotation(r);
        m_num_txt.rotate(90);
    }

    int getIndex() {
        return m_num;
    }

    bool contains(int x, int y) {
        return m_tile.contains(x, y);
    }

    virtual void draw (RenderTarget &target, RenderStates states) const override {
        // First draw the tile
        target.draw(m_tile, states);
        // Then the text
        target.draw(m_num_txt, states);
    }
private:
    void updateColours() {
        if (m_filled) {
            m_tile.setFillColor(Colours::tile_to_col(m_tile.colour())); 
            m_tile.setOutlineThickness(0.0);    
        } else {
            if (m_hovered) {
                m_tile.setFillColor(Colours::board_tile_unfill_hover_col(m_tile.colour()));
            } else {
                m_tile.setFillColor(Colours::board_tile_unfill_col(m_tile.colour()));
            }
            m_tile.setOutlineColor(Color::Black);
            m_tile.setOutlineThickness(2.0);
        }
    }
    Tile m_tile;
    sf::Font m_font;
    int m_num;
    bool m_filled;
    bool m_hovered;
    sf::Text m_num_txt;
};

#endif