#ifndef BUTTON
#define BUTTON

#include <SFML/Graphics.hpp>

class Button {
public:
    Button (sf::Texture* normal, sf::Texture* clicked, std::string, sf::Vector2f location);
    bool onClick(int x, int y);
    void setState(bool);
    void setText(std::string);
    bool getVar();
    sf::Sprite* getSprite();
private:
    sf::Sprite m_normal;
    sf::Sprite m_clicked;
    sf::Sprite* m_currentSpr;
    sf::Text m_string;
    bool m_current;
};

#endif
