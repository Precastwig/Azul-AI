#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <ui_elements/Button.hpp>

using namespace sf;

class MainMenu : public Drawable {
public:
    MainMenu(sf::RenderWindow* window, sf::Vector2f size = sf::Vector2f());
    ~MainMenu() {};

    enum MenuState {
        MAIN,
        SETTINGS,
        OFF
    };

    virtual void draw (RenderTarget &target, RenderStates states) const override;
    void onClick(int xPos, int yPos);
    // Public so that we can give it the callback easily
    Button m_newgame;
private:
    // Render window
    sf::RenderWindow* m_window;

    MenuState m_state;

    // Background?

    // Main state
    Button m_settings;
    Button m_exit;

    // Settings state
};