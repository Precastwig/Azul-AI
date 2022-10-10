#include <ui_elements/MainMenu.hpp>
#include <cstdlib>
#include "Logger.hpp"
#include "utils/Choices.hpp"
#include "utils/Sounds.hpp"
#include "utils/helper_enums.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Game.hpp"
#if __linux__
#include <X11/Xlib.h>
#endif

enum gui_modes {
	COMMAND_LINE,
	QT
};

// Prints a bunch of info to the command line
Logger g_logger;
sf::Font g_font;
MenuState g_menu_state;
Sounds g_sounds;

int main(int argc, char *argv[]) {
	#if __linux__
		XInitThreads();
	#endif
	gui_modes guiMode = QT;
	// Initialise random seed
	srand(time(nullptr));
	// Parse flags
	int autoMode = -1;
	for (int i = 1; i < argc; i++) {
		std::string arg(argv[i]);
		if (autoMode > 0) {
			if (arg == "rand" || arg == "random") {
				autoMode--;
			}
			if (arg == "colrand") {
				autoMode--;
			}
		}
		if (arg == "-cmd") {
			guiMode = COMMAND_LINE;
		} else if (arg == "-qt") {
			guiMode = QT;
		} else if (arg == "-v") {
			g_logger.enable();
		} else if (arg == "-auto") {
			autoMode = 2;
			// We expect the next two arguments to be the two types of AI to play
		}
	}
	if (guiMode == COMMAND_LINE) {
		// COMMAND LINE MODE DEPRICATED BECAUSE I'M LAZY
		// It /probably/ works, but all the UI elements would be created
		// and moved around, so you'd just be saving the rendering time,
		// Which I imagine is minimal.
		// Game game;
		// game.play();
	} else if (guiMode == QT) {
		
		if (!g_font.loadFromFile("resources/NotoSansCJK-Medium.ttc")) {
            g_logger.log(Logger::ERROR, "Font not loaded");
        }
		int window_width = 1920;
		int window_height = 1080;
		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;
		sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Azul: Summer Pavillion", sf::Style::Titlebar | sf::Style::Close, settings);
		window.setFramerateLimit(120); // 120 seems like plenty
		MainMenu menu(&window, sf::Vector2f(window_width, window_height));
	
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
						menu.onClick(event.mouseButton.x, event.mouseButton.y);
					}
				}

				if (event.type == sf::Event::MouseWheelScrolled) {
					if (event.mouseWheelScroll.delta > 0) {
						// Positive is up/left
						menu.onLeft();
					} else {
						menu.onRight();
					}
				} else if (event.type == sf::Event::KeyPressed) {
					if (event.key.code == sf::Keyboard::Key::Left || 
					event.key.code == sf::Keyboard::Key::A || 
					event.key.code == sf::Keyboard::Key::W) {
						menu.onLeft();
					} else if (event.key.code == sf::Keyboard::Key::Right || 
					event.key.code == sf::Keyboard::Key::D || 
					event.key.code == sf::Keyboard::Key::S) {
						menu.onRight();
					}
				}

				if (event.type == sf::Event::MouseMoved) {
					menu.onHover(event.mouseMove.x, event.mouseMove.y);
				}
	        }
	        // Clear screen
	        window.clear(sf::Color(194, 240, 242));
	        // Draw the relevent thing
			menu.performMultithreadedActions();
			window.draw(menu);
	        // Update the window
	        window.display();
	    }
	    return EXIT_SUCCESS;
	}
};
