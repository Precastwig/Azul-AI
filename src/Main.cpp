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
	gui_modes guiMode = QT;
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
	Game game;
	if (guiMode == COMMAND_LINE) {
		game.play();
	} else if (guiMode == QT) {
		// Unsupported for now
		int window_width = 1000;
		int window_height = 1000;
		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;
		sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Azul: Summer Pavillion", sf::Style::Default, settings);
		// Create a graphical text to display
	   sf::Font font;
	   if (!font.loadFromFile("/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf"))
		   return EXIT_FAILURE;
	   sf::Text text("Hello SFML", font, 50);

	   // Game stuff
	   game.fill_factories();
	   // Start the game loop
	    while (window.isOpen())
	    {
	        // Process events
	        sf::Event event;
	        while (window.pollEvent(event))
	        {
	            // Close window: exit
	            if (event.type == sf::Event::Closed)
	                window.close();
	        }
	        // Clear screen
	        window.clear(Color(179, 157, 114));
	        // Draw the string
	        window.draw(game);
	        // Update the window
	        window.display();
	    }
	    return EXIT_SUCCESS;
	}
};
