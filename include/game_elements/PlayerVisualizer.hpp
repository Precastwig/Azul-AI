#ifndef PLAYERVISUALIZER
#define PLAYERVISUALIZER

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>
#include "Logger.hpp"
#include "players/Player.hpp"

extern Logger g_logger;

class PlayerVisualizer : public sf::Drawable {
public:
    PlayerVisualizer(std::shared_ptr<Player> player, sf::Vector2f location, sf::Vector2f size);

    virtual void draw (sf::RenderTarget &target, sf::RenderStates states) const override;
    void update(std::shared_ptr<Player> currentPlayer);
private:
    void updateString();
    // The thing we're visualizing
    std::shared_ptr<Player> m_player;
    // Visualization elements
    sf::Text m_player_name;
    sf::RectangleShape m_outline;
    // Should font be a global? maybe
    sf::Font m_font;

    // I think we want a hover state? Maybe later
    bool m_hovering;
};

#endif