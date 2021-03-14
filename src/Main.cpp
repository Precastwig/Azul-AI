#include <SFML/Graphics.hpp>
#include "Game.hpp"

int main() {
	int window_width = 1000;
    int window_height = 1000;
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Azul: Summer Pavillion");
	Game game(2);
	game.play();
};
