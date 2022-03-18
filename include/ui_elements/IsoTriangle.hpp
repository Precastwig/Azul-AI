#ifndef ISOTRIANGLE
#define ISOTRIANGLE

#include <cmath>
#include <ui_elements/CustomShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/System/Vector2.hpp>

class IsoTriangle : public CustomShape {
public:
    IsoTriangle() : m_size(60), m_angle(60) {
        initialise_points();
    };
    // Angle is in radians!
    IsoTriangle(float size, float angle) : m_size(size), m_angle(angle) {
        initialise_points();
    };
private:
    void initialise_points() {
        setPointCount(3);
        float half_angle = m_angle / 2.0;
        setPoint(0, sf::Vector2f(0,0));
        float vertical_length = m_size * std::cos(half_angle);
        float horizontal_half_length = m_size * std::sin(half_angle);
        setPoint(1,sf::Vector2f(horizontal_half_length, vertical_length));
        setPoint(2,sf::Vector2f(-horizontal_half_length, vertical_length));
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
    float m_size;
    float m_angle;
};

#endif