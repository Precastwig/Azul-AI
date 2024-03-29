#ifndef PLAYERVISUALIZER
#define PLAYERVISUALIZER

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>
#include "Logger.hpp"
#include "players/Player.hpp"
#include "utils/helper_enums.hpp"
#include <ui_elements/Button.hpp>

extern Logger g_logger;
extern GameState g_visual_state;

class PlayerVisualizer : public sf::Drawable {
public:
    PlayerVisualizer(std::shared_ptr<Player> player, sf::Vector2f location, sf::Vector2f size);

    virtual void draw (sf::RenderTarget &target, sf::RenderStates states) const override;
    void onClick(int x, int y);
    void onHover(int x, int y);
    bool contains(int x, int y);
    std::shared_ptr<Player> getPlayerVisualized();
    
    void update();
private:
    void updateString();
    void donePlacing();
    void updatePlayerTilePositions();
    // The thing we're visualizing
    std::shared_ptr<Player> m_player;
    // Visualization elements
    GameState* m_game_state;
    sf::Text m_player_name;
    sf::RectangleShape m_outline;
    Button m_doneplacing_button;
    FirstTile m_first_tile;

    // I think we want a hover state? Maybe later
    bool m_hovering;
};

#endif