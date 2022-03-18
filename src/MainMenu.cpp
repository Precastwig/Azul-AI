#include "players/RandomAI.hpp"
#include "ui_elements/Selector.hpp"
#include "utils/helper_enums.hpp"
#include <ui_elements/MainMenu.hpp>
#include <SFML/System/Vector2.hpp>

extern MenuState g_menu_state;

MainMenu::MainMenu(sf::RenderWindow* window, sf::Vector2f size) : m_window(window), m_newgame("New Game"), m_settings("Settings"), m_exit("Quit"), m_start_new_game("Start Game"), m_num_players(SelectorType::SCROLL) {
    int xpos = size.x / 10.0;
    g_menu_state = MAIN;
    m_newgame.setPosition(sf::Vector2f(xpos, size.y / 4));
    m_newgame.setOutlineThickness(0.0);
    m_settings.setPosition(sf::Vector2f(xpos, (size.y * 2) / 4));
    m_settings.setOutlineThickness(0.0);
    m_exit.setPosition(sf::Vector2f(xpos, (size.y * 3) / 4));
    m_exit.setOutlineThickness(0.0);

    m_start_new_game.setPosition(sf::Vector2f(xpos, (size.y * 3) / 4));
    m_start_new_game.setOutlineThickness(0.0);
    m_num_players.setPosition(sf::Vector2f(xpos, size.y / 4));
    m_num_players.addOption("2", 2);
    m_num_players.addOption("3", 3);
    m_num_players.addOption("4", 4);
    updatePlayerSelectors();
}

void MainMenu::performMultithreadedActions() {
    if (g_menu_state == MenuState::OFF && m_game) {
        m_game->performAIActions();
    }
}

void MainMenu::onHover(int x, int y) {
    if (g_menu_state == MenuState::OFF && m_game) {
        m_game->onHover(x, y);
    }

    if (g_menu_state == MenuState::MAIN) {
        m_newgame.setHoverState(m_newgame.contains(x, y));
        m_exit.setHoverState(m_exit.contains(x, y));
        m_settings.setHoverState(m_settings.contains(x, y));
    }
}

void MainMenu::onClick(int xPos, int yPos) {
    if (g_menu_state == MenuState::OFF) {
        m_game->onClick(xPos, yPos);
        return;
    }

    if (g_menu_state == MenuState::MAIN) {
        if (m_newgame.onClick(xPos, yPos)) {
            // Do newgame thing
            g_menu_state = NEWGAME;
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

    if (g_menu_state == MenuState::NEWGAME) {
        if (m_start_new_game.onClick(xPos, yPos)) {
            sf::Vector2u windowsize = m_window->getSize();
            std::vector<PlayerType> player_types = getPlayerTypesFromUI();
            m_game = std::make_unique<Game>(player_types, sf::Vector2f(windowsize.x, windowsize.y));
            g_menu_state = MenuState::OFF;
        } else if (m_num_players.onClick(xPos, yPos)) {
            updatePlayerSelectors();
        }
        for (Selector<PlayerType>& s : m_player_type_selectors) {
            if (s.onClick(xPos, yPos)) {
                return;
            }
        }
    }
}

std::vector<PlayerType> MainMenu::getPlayerTypesFromUI() {
    std::vector<PlayerType> types;
    for (Selector<PlayerType>& player_selector : m_player_type_selectors) {
        types.push_back(player_selector.getSelectedVal());
    }
    return types;
}

void MainMenu::updatePlayerSelectors() {
    int num_players = m_num_players.getSelectedVal();
    while (m_player_type_selectors.size() < num_players) {
        // We need to create a new one
        sf::Vector2f position(m_window->getSize().x * 6.0 / 10, m_window->getSize().y * (3 + m_player_type_selectors.size())/ 10.0);
        Selector<PlayerType> newSelector(SelectorType::SCROLL, position);
        newSelector.addOption("Human", HUMAN);
        newSelector.addOption("Random AI", RANDOM);
        m_player_type_selectors.push_back(newSelector);
    }

    while (m_player_type_selectors.size() > num_players) {
        // Remove the last one
        m_player_type_selectors.pop_back();
    }
}

void MainMenu::draw (RenderTarget &target, RenderStates states) const {
    switch (g_menu_state) {
    case OFF:
        if (m_game) {
            target.draw(*m_game);
        }
        return;
    case NEWGAME:
        target.draw(m_num_players);
        for (const Selector<PlayerType>& s : m_player_type_selectors) {
            target.draw(s);
        }
        target.draw(m_start_new_game);
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