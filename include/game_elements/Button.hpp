#ifndef BUTTON
#define BUTTON

#include <SFML/Graphics.hpp>
#include "Logger.hpp"

extern Logger g_logger;

class Button : public sf::Drawable {
public:
    Button(std::string str, sf::Vector2f location);
    // Button (sf::Texture* normal, sf::Texture* clicked, std::string, sf::Vector2f location);

	virtual void draw (sf::RenderTarget &target, sf::RenderStates states) const override;

    bool onClick(int x, int y);
    void setState(bool);
    void setText(std::string);
    bool getVar();
    // sf::Sprite* getSprite();
private:
    // sf::Sprite m_normal;
    // sf::Sprite m_clicked;
    // sf::Sprite* m_currentSpr;
    sf::RectangleShape m_outline;
    sf::Text m_string;
    sf::Font m_font;
    bool m_current;
};

#endif
