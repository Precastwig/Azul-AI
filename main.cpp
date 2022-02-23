#include <SFML/Graphics.hpp>
#include "Game.hpp"
#include "MainMenu.hpp"
#include "Logger.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <iostream>

enum gui_modes {
	COMMAND_LINE,
	QT
};

// Prints a bunch of info to the command line
Logger g_logger;

int main(int argc, char *argv[]) {
	gui_modes guiMode = QT;
	// Parse flags
	for (int i = 1; i < argc; i++) {
		std::string arg(argv[i]);
		if (arg == "-cmd") {
			guiMode = COMMAND_LINE;
		} else if (arg == "-qt") {
			guiMode = QT;
		} else if (arg == "-v") {
			g_logger.enable();
		}
	}
	if (guiMode == COMMAND_LINE) {
		Game game;
		game.play();
	} else if (guiMode == QT) {
		int window_width = 1920;
		int window_height = 1080;
		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;
		sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Azul: Summer Pavillion", sf::Style::Titlebar | sf::Style::Close, settings);
		Game game(sf::Vector2f(window_width, window_height));
	   	// Game stuff
	   	game.fill_factories();
		bool menuopen = true;
		MainMenu menu(&window, sf::Vector2f(window_width, window_height));
	   	menu.m_newgame.m_callback = [&menuopen]() {menuopen = !menuopen;};
	
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

				if (event.type == sf::Event::MouseButtonPressed) {
					if (event.mouseButton.button == sf::Mouse::Left) {
						if (menuopen) {
							// Give to the menu
							menu.onClick(event.mouseButton.x, event.mouseButton.y);
						} else {
							game.onClick(event.mouseButton.x, event.mouseButton.y);
						}
					}
				}

				if (event.type == sf::Event::MouseMoved) {
					if (menuopen) {
						// Menu onhover
					} else {
						game.onHover(event.mouseMove.x, event.mouseMove.y);
					}
				}
	        }
	        // Clear screen
	        window.clear(Color(194, 240, 242));
	        // Draw the relevent thing
			if (menuopen) {
				window.draw(menu);
			} else {
				// This is non locking, so shouldn't halt UI
				game.performAIActions();
	        	window.draw(game);
			}
	        // Update the window
	        window.display();
	    }
	    return EXIT_SUCCESS;
	}
};
