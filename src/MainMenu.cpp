#include <ui_elements/MainMenu.hpp>
#include <SFML/System/Vector2.hpp>

extern MenuState g_menu_state;

MainMenu::MainMenu(sf::RenderWindow* window, sf::Vector2f size) : m_window(window), m_newgame("New Game"), m_settings("Settings"), m_exit("Quit") {
    int xpos = size.x / 10.0;
    g_menu_state = MAIN;
    m_newgame.setPosition(sf::Vector2f(xpos, size.y / 4));
    m_newgame.setOutlineThickness(0.0);
    m_settings.setPosition(sf::Vector2f(xpos, (size.y * 2) / 4));
    m_settings.setOutlineThickness(0.0);
    m_exit.setPosition(sf::Vector2f(xpos, (size.y * 3) / 4));
    m_exit.setOutlineThickness(0.0);
}

void MainMenu::onClick(int xPos, int yPos) {
    if (m_newgame.onClick(xPos, yPos)) {
        // Do newgame thing
        g_menu_state = OFF;
        g_logger.log(Logger::INFO, "New game button pressed");
    } else if (m_exit.onClick(xPos, yPos)) {
        // Quit game
        m_window->close();
        g_logger.log(Logger::INFO, "Quit game button pressed");
    } else if (m_settings.onClick(xPos, yPos)) {
        // Open settings sub menu
        g_menu_state = SETTINGS;
        g_logger.log(Logger::INFO, "Settings game button pressed");
    }
}

void MainMenu::draw (RenderTarget &target, RenderStates states) const {
    switch (g_menu_state) {
    case OFF:
        return;
    case MAIN:
        target.draw(m_newgame);
        target.draw(m_settings);
        target.draw(m_exit);
        return;
    case SETTINGS:

        return;
    }
}