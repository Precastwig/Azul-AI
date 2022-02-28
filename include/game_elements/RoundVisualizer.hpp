#ifndef ROUNDVISUALIZER
#define ROUNDVISUALIZER

#include <SFML/Graphics/Drawable.hpp>
#include <game_elements/Tile.hpp>

class RoundVisualizer : sf::Drawable {
public:
    RoundVisualizer(std::vector<TileType> order, sf::Vector2f position);
    ~RoundVisualizer() {};
    virtual void draw (RenderTarget &target, RenderStates states) const override;
    void nextround();
private:
    void updateVisuals();
    int m_current_round;
    std::vector<Tile> m_bonusmarkers;
    std::vector<sf::Text> m_roundmarkers;
};

#endif