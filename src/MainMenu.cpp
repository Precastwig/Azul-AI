#include "players/RandomAI.hpp"
#include "ui_elements/Selector.hpp"
#include "utils/helper_enums.hpp"
#include <algorithm>
#include <ui_elements/MainMenu.hpp>
#include <SFML/System/Vector2.hpp>
#include <utility>

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
        return;
    }

    if (g_menu_state == MenuState::NEWGAME) {
        if (m_start_new_game.onClick(xPos, yPos)) {
            sf::Vector2u windowsize = m_window->getSize();
            std::vector<std::pair<PlayerType, PlayerColour::Colour>> players = getPlayerInfoFromUI();
            // Check the player info is good
            std::vector<PlayerColour::Colour> used_colours;
            bool invalid = false;
            for (auto& p_info : players) {
                if (std::find(used_colours.begin(), used_colours.end(), p_info.second) != used_colours.end()) {
                    // One colour used twice
                    invalid = true; 
                }
                used_colours.push_back(p_info.second);
            }
            if (!invalid) {
                m_game = std::make_unique<Game>(players, sf::Vector2f(windowsize.x, windowsize.y));
                g_menu_state = MenuState::OFF;
            } else {
                // Do a warning thing?!?!?!??!?! maybe
            }
        } else if (m_num_players.onClick(xPos, yPos)) {
            updatePlayerSelectors();
        }
        for (Selector<PlayerType>& s : m_player_type_selectors) {
            if (s.onClick(xPos, yPos)) {
                return;
            }
        }
        for (Selector<PlayerColour::Colour>& c : m_player_colour_selectors) {
            if (c.onClick(xPos, yPos)) {
                return;
            }
        }
    }
}

std::vector<std::pair<PlayerType, PlayerColour::Colour>> MainMenu::getPlayerInfoFromUI() {
    std::vector<std::pair<PlayerType, PlayerColour::Colour>> types;
    for (size_t i = 0; i < m_player_type_selectors.size(); ++i) {
        PlayerColour::Colour col; 
        if (i >= m_player_colour_selectors.size()) {
            // This shouldn't happen
            g_logger.log(Logger::ERROR, "Different number of player type selectors as player colour selectors");
            col = PlayerColour::BLACK; // IDK
        } else {
            col = m_player_colour_selectors[i].getSelectedVal();
        }

        types.push_back(std::pair<PlayerType, PlayerColour::Colour>(m_player_type_selectors[i].getSelectedVal(), col));
    }
    return types;
}

void MainMenu::updatePlayerSelectors() {
    int num_players = m_num_players.getSelectedVal();
    while (m_player_type_selectors.size() < num_players) {
        // We need to create a new one 
        float player_y = m_window->getSize().y * (3 + m_player_type_selectors.size())/ 10.0;
        sf::Vector2f position(m_window->getSize().x * 6.0 / 10.0, player_y);
        Selector<PlayerType> newTypeSelector(SelectorType::SCROLL, position);
        newTypeSelector.addOption("Human", HUMAN);
        newTypeSelector.addOption("Random AI", RANDOM);
        newTypeSelector.addOption("Colour Target AI", COLOURTARGET);
        m_player_type_selectors.push_back(newTypeSelector);
        float colour_x = position.x - (newTypeSelector.getWidth() * 1.5);
        Selector<PlayerColour::Colour> newColourSelector(SelectorType::SCROLL,sf::Vector2f(colour_x, player_y));
        for (PlayerColour::Colour c : PlayerColour::all_colours()) {
            newColourSelector.addOption(PlayerColour::toString(c), c);
        }
        m_player_colour_selectors.push_back(newColourSelector);
    }

    while (m_player_type_selectors.size() > num_players) {
        // Remove the last one
        m_player_type_selectors.pop_back();
        m_player_colour_selectors.pop_back();
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
        for (const Selector<PlayerColour::Colour>& c: m_player_colour_selectors) {
            target.draw(c);
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