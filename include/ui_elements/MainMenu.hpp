#include "ui_elements/Selector.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <ui_elements/Button.hpp>
#include <Game.hpp>
#include <utils/helper_enums.hpp>

using namespace sf;

class MainMenu : public Drawable {
public:
    MainMenu(sf::RenderWindow* window, sf::Vector2f size = sf::Vector2f());
    ~MainMenu() {};

    virtual void draw (RenderTarget &target, RenderStates states) const override;
    void onClick(int xPos, int yPos);
    void onHover(int x, int y);
    void onLeft() {
        // also up 
        m_game->onLeft();
    }
    void onRight() {
        // also down
        m_game->onRight();
    }
    void performMultithreadedActions();


    // Public so that we can give it the callback easily
    Button m_newgame;
private:
    void updatePlayerSelectors();
    std::vector<std::pair<PlayerType, PlayerColour::Colour>> getPlayerInfoFromUI();
    // Render window
    sf::RenderWindow* m_window;

    // Game state
    std::unique_ptr<Game> m_game;

    // New game options
    Selector<int> m_num_players;
    std::vector<Selector<PlayerType>> m_player_type_selectors;
    std::vector<Selector<PlayerColour::Colour>> m_player_colour_selectors;
    Button m_start_new_game;

    // Main state
    Button m_settings;
    Button m_exit;

    // Settings state
};