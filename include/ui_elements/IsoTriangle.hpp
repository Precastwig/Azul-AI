#ifndef ISOTRIANGLE
#define ISOTRIANGLE

#include "utils/Sounds.hpp"
#include <cmath>
#include <ui_elements/CustomShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/System/Vector2.hpp>

class IsoTriangle : public CustomShape {
public:
    IsoTriangle() : m_size(60), m_angle(60), m_hovered(false) {
        initialise_points();
        setOutlineColor(sf::Color::White);
    };
    // Angle is in radians!
    IsoTriangle(float size, float angle) : m_size(size), m_angle(angle) {
        initialise_points();
        setOutlineColor(sf::Color::White);
    };
    // This is the size of the mirrored edge
    const float get_size() {
        return m_size;
    }
    const float get_height() {
        return m_vertical_length;
    }
    void onHover(int x, int y) {
        if (contains(x,y)) {
            if (!m_hovered) {
                m_hovered = true;
                setOutlineThickness(5.0);
                Sounds::click(3.0);
            }
        } else {
            if (m_hovered) {
                setOutlineThickness(0.0);
                m_hovered = false;
            }
        }
    }
private:
    void initialise_points() {
        setPointCount(3);
        float half_angle = m_angle / 2.0;
        setPoint(0, sf::Vector2f(0,0));
        m_vertical_length = m_size * std::cos(half_angle);
        float horizontal_half_length = m_size * std::sin(half_angle);
        setPoint(1,sf::Vector2f(horizontal_half_length, m_vertical_length));
        setPoint(2,sf::Vector2f(-horizontal_half_length, m_vertical_length));
        update();
    }
    //   o = m_angle
    //  
    //       ^
    //      /o\
    //     /   \
    //    /     \m_size
    //   /       \
    //  /         \
    //  -----------
    //     
    float m_vertical_length;
    float m_size;
    float m_angle;
    bool m_hovered;
};

#endif 