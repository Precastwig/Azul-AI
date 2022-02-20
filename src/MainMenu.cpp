#include "MainMenu.hpp"
#include <SFML/System/Vector2.hpp>

MainMenu::MainMenu(sf::RenderWindow* window, sf::Vector2f size) : m_window(window), m_state(MAIN), m_newgame("New Game"), m_settings("Settings"), m_exit("Quit") {
    int xpos = size.x / 10.0;
    m_newgame.setPosition(sf::Vector2f(xpos, size.y / 4));
    m_newgame.setOutline(true);
    m_settings.setPosition(sf::Vector2f(xpos, (size.y * 2) / 4));
    m_settings.setOutline(true);
    m_exit.setPosition(sf::Vector2f(xpos, (size.y * 3) / 4));
    m_exit.setOutline(true);
}

void MainMenu::onClick(int xPos, int yPos) {
    if (m_newgame.onClick(xPos, yPos)) {
        // Do newgame thing
        m_state = OFF;
        g_logger.log(Logger::INFO, "New game buttone pressed");
    } else if (m_exit.onClick(xPos, yPos)) {
        // Quit game
        m_window->close();
        g_logger.log(Logger::INFO, "Quit game button pressed");
    } else if (m_settings.onClick(xPos, yPos)) {
        // Open settings sub menu
        m_state = SETTINGS;
        g_logger.log(Logger::INFO, "Settings game button pressed");
    }
}

void MainMenu::draw (RenderTarget &target, RenderStates states) const {
    switch (m_state) {
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