#include <SFML/Graphics.hpp>
#include "Game.hpp"
#include <iostream>

enum gui_modes {
	COMMAND_LINE,
	QT
};

// Prints a bunch of info to the command line
bool DEBUG_MODE = true;

int main(int argc, char *argv[]) {
	gui_modes guiMode = COMMAND_LINE;
	// Parse flags
	for (int i = 1; i < argc; i++) {
		std::string arg(argv[i]);
		if (arg == "-cmd") {
			guiMode = COMMAND_LINE;
		} else if (arg == "-qt") {
			guiMode = QT;
		} else if (arg == "-debug") {
			DEBUG_MODE = true;
		}
	}
	if (guiMode == COMMAND_LINE) {
		Game game;
		game.play();
	} else if (guiMode == QT) {
		// Unsupported for now
		int window_width = 1000;
		int window_height = 1000;
		sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Azul: Summer Pavillion");
	}
};
