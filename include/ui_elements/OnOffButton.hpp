#ifndef ONOFFBUTTON
#define ONOFFBUTTON

#include <SFML/Graphics/Color.hpp>
#include <ui_elements/Button.hpp>
#include <Colours.hpp>

class OnOffButton : public Button {
public:
    OnOffButton(std::string str = "", sf::Vector2f location = sf::Vector2f(0,0)) : Button(str, location), m_on(false) {

    }
    virtual bool onClick(int x, int y) override {
        std::cout << "onoff onclick\n";
        bool clicked = Button::onClick(x, y);
        if (clicked) {
            g_logger.log(Logger::INFO, "Clicked on/off button");
            setOnState(true);
        }
        return clicked;
    }
    bool getState() {
        return m_on;
    }
    void setOnState(bool on) {
        m_on = on;
        updateColours();
    }

protected:
    // Update style in response to m_on var
    virtual void updateColours() override {
        if (m_on) {
            m_background.setFillColor(Colours::grey());
        } else {
            m_background.setFillColor(sf::Color::Blue);
            Button::updateColours();
        }
    }

    std::string m_onString;
    std::string m_offString;
    sf::Color m_onColour;
    sf::Color m_offColour;
    bool m_onOutline;
    bool m_offOutline;
private:
    // On or off?!?
    bool m_on;
};

#endif