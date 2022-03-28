#ifndef BUTTON
#define BUTTON

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include "Logger.hpp"

extern Logger g_logger;

class Button : public sf::Drawable {
public:
    Button(std::string str = "", sf::Vector2f location = sf::Vector2f(0,0));
    // Button (sf::Texture* normal, sf::Texture* clicked, std::string, sf::Vector2f location);

	virtual void draw (sf::RenderTarget &target, sf::RenderStates states) const override;
    bool onClick(int x, int y);
    void onHover(int x, int y);

    bool contains(int x, int y);
    void setText(std::string);
    void setSize(sf::Vector2f size);
    void setOutlineThickness(float size) {m_background.setOutlineThickness(size);};
    void setPosition(sf::Vector2f newpos);
    void setColour(sf::Color col);
    
    std::function<void()> m_callback;
    // sf::Sprite* getSprite();
protected:
    virtual void updateColours();
    // sf::Sprite m_normal;
    // sf::Sprite m_clicked;
    // sf::Sprite* m_currentSpr;
    sf::RectangleShape m_background;
    bool m_hovered;
    sf::Text m_string;
};

#endif
