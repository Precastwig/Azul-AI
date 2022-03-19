#ifndef FINISHSCREEN
#define FINISHSCREEN

#include "players/Player.hpp"
#include "ui_elements/Button.hpp"
#include "utils/helper_enums.hpp"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <vector>

extern MenuState g_menu_state;
extern sf::Font g_font;

class FinishScreen : sf::Drawable {
public:
    FinishScreen(sf::Vector2f size, std::vector<std::shared_ptr<Player>> players) {
        std::sort(players.begin(), players.end(), [](std::shared_ptr<Player> p1, std::shared_ptr<Player> p2) {
            return p1->points() > p2->points();
        });
        sf::Vector2f startpos(size.x/2.0, size.y*1/8.0);
        for (size_t i = 0; i < players.size(); ++i) {
            sf::Text txt;
            txt.setFont(g_font);
            std::string str = getPlaceString(i) + players[i]->toShortString();
            txt.setPosition(startpos);
            txt.setColor(sf::Color::Black);
            txt.move(sf::Vector2f(0, i * size.y / 8.0));
            txt.setString(str);
            m_scores.push_back(txt);
        }
        m_ok.setText("Back to menu");
        m_ok.setPosition(sf::Vector2f(size.x/2.0, size.y*4/5));
        m_ok.m_callback = []() {
            g_menu_state = MenuState::MAIN;
            // Kill the game here?
            // Or dont...
        };
    }
    void onClick(int x, int y) {
        m_ok.onClick(x, y);
    }
    virtual void draw (sf::RenderTarget &target, sf::RenderStates states) const override {
        for (size_t i = 0; i < m_scores.size(); ++i) {
            target.draw(m_scores[i]);
        }
        target.draw(m_ok, states);
    }
private:
    std::string getPlaceString(int place) {
        if (place == 0) {
            return "1st: ";
        }
        if (place == 1) {
            return "2nd: ";
        }
        if (place == 2) {
            return "3rd: "; 
        }
        if (place == 3) {
            return "4th: ";
        }
        return "5th?";
    } 
    std::vector<sf::Text> m_scores;
    Button m_ok;
};

#endif