#include <SFML/Graphics.hpp>
#include "Game.hpp"
#include <iostream>

int main() {
	int in;
	do {
		std::cout << "Choose an option:\n1. Command line play against random AI\n2. UI play against random AI (unfinished)\n";
		std::cin >> in;
	} while (in != 1);
	// only allow cmd play for now...
	if (in == 2) {
		int window_width = 1000;
		int window_height = 1000;
		sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Azul: Summer Pavillion");
	}
	Game game;
	game.play();
};
