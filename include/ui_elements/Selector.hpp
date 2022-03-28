#ifndef SELECTOR
#define SELECTOR

#include "utils/cIndex.hpp"
#include <SFML/System/Vector2.hpp>
#include <ui_elements/IsoTriangle.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Color.hpp>

enum SelectorType {
    SCROLL,
    LIST // tbd
};

extern sf::Font g_font;

template <class A>
class Selector : public sf::Drawable {
public:
    Selector(SelectorType t = SCROLL, sf::Vector2f location = sf::Vector2f(0,0)) : m_style(t), m_next_triangle(80, arrow_angle), m_prev_triangle(80, arrow_angle), m_position(location), m_selected(1,1) {
        m_next_triangle.setRotation(90);
        m_next_triangle.setFillColor(sf::Color::Black);
        m_prev_triangle.setRotation(270);
        m_prev_triangle.setFillColor(sf::Color::Black);
        updatePositions();
    }
    virtual void draw (sf::RenderTarget &target, sf::RenderStates states) const override  {
        if (m_style == SCROLL) {
            target.draw(m_prev_triangle, states);
            target.draw(m_next_triangle, states);
            if (!m_options.empty()) {
                target.draw(m_options[m_selected.getIndex()], states);
            }
        } else {
            for (size_t i = 0; i < m_options.size(); ++i) {
                target.draw(m_options[i], states);
            }
        }
    }
    bool onClick(int x, int y) {
        if (m_style == SCROLL) {
            if (m_next_triangle.contains(x, y)) {
                m_selected++;
                return true;
            }
            if (m_prev_triangle.contains(x, y)) {
                m_selected--;
                return true;
            }
        } else if (m_style == LIST) {
            for (size_t i = 0; i < m_options.size(); ++i) {
                if (m_options[i].getGlobalBounds().contains(x, y)) {
                    m_options[i].setStyle(sf::Text::Bold | sf::Text::Underlined);
                    m_selected.set_index(i);
                } else {
                    m_options[i].setStyle(sf::Text::Regular);
                }
            }
        }
        return false;
    }

    void onHover(int x, int y) {
        m_prev_triangle.onHover(x, y);
        m_next_triangle.onHover(x, y);
    }

    float getWidth() {
        if (m_style == SCROLL) {
            return m_text_width + m_prev_triangle.get_height() + m_next_triangle.get_height();
        }
        return m_text_width; 
    }

    void setPosition(sf::Vector2f pos) {
        m_position = pos;
        updatePositions();
    }
    void addOption(std::string str, A val) {
        sf::Text newT;
        newT.setFont(g_font);
        newT.setColor(sf::Color::Black);
        newT.setString(str);
        sf::FloatRect playerRect = newT.getLocalBounds();
        newT.setOrigin(playerRect.left + playerRect.width/2.0f,
                            playerRect.top  + playerRect.height/2.0f);
        m_options.push_back(newT);
        m_values.push_back(val);
        m_selected.set_max(m_options.size());
        updatePositions();
    }
    int getSelectedIndex() {return m_selected.getIndex();}
    A getSelectedVal() {return m_values[getSelectedIndex()];}
private:
    const float arrow_angle = M_PI / 3;
    void updatePositions() {
        m_text_width = 0; // max text width
        for (size_t i = 0; i < m_options.size(); ++i) {
            m_options[i].setPosition(position_from_index(i));
            if (m_options[i].getGlobalBounds().width > m_text_width) {
                m_text_width = m_options[i].getGlobalBounds().width;
            }
        }
        m_next_triangle.setPosition(sf::Vector2f(m_position.x + m_text_width + m_next_triangle.get_height(), m_position.y));
        m_prev_triangle.setPosition(sf::Vector2f(m_position.x - m_text_width - m_prev_triangle.get_height(), m_position.y));
    }
    sf::Vector2f position_from_index(int index) {
        if (m_style == SCROLL) {
            return m_position;
        } else if (m_style == LIST) {
            return sf::Vector2f(m_position.x, m_position.y + (index * 25));
        }
        return m_position;
    }
    float m_text_width;
    cIndex m_selected;
    std::vector<sf::Text> m_options;
    std::vector<A> m_values;
    SelectorType m_style;
    sf::Vector2f m_position;

    // For scroll type only
    IsoTriangle m_prev_triangle;
    IsoTriangle m_next_triangle;
};

#endif