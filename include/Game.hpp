#ifndef GAME
#define GAME

#include <SFML/System/Vector2.hpp>
#include <vector>
#include <memory>
#include <mutex>
#include "players/Player.hpp"
#include "game_elements/Bag.hpp"
#include "game_elements/Factory.hpp"
#include "game_elements/Button.hpp"
#include "game_elements/PlayerVisualizer.hpp"
#include <SFML/Graphics.hpp>

using namespace sf;

class Game : public Drawable {
public:
	// Constructor/Destructor
	Game(sf::Vector2f size = sf::Vector2f());
	~Game();

	virtual void draw (RenderTarget &target, RenderStates states) const override;

	void onClick(int xPos, int yPos);

	
	//-------------------------------------------------------------------
	// SFML specific functions
	void performAIActions();
	void performAIAction(std::shared_ptr<Player> player);

	//-------------------------------------------------------------------
	// Command line specific functions
	// Initiates the play sequence
	void play();
	// Prints the factories to stdout
	void printFactories();
	// Print some info to stdout declaring the winner of the game
	void declare_winner();

	// Turn step functions

	// Pulls tiles from the bag and places on the factories until each
	// factory contains 4, or the bag runs out
	void fill_factories();
	// Players take turns picking tiles from factories until all are empty
	void picking_stage();
	void pick_tile(PickingChoice& picked);
	// Players take turns placing tiles on their personal boards until they choose to stop (or can't place any more tiles)
	void placing_stage();

	//-------------------------------------------------------------------
	// Helper functions

	// Check if all factories are empty
	bool noTilesLeft();
	// Check that there is a player that still wishes to place tiles
	bool playerNotFinished();
	// Get the current bonus tile type
	Tile::Type getBonus();

private:
	//-------------------------------------------------------------------
	// Member variables
	// Six rounds
	std::vector<Tile::Type> m_bonus_tile_order = {
		Tile::PURPLE,
		Tile::GREEN,
		Tile::ORANGE,
		Tile::YELLOW,
		Tile::BLUE,
		Tile::RED
	};

	// Tile sources
	std::shared_ptr<Bag> m_bag;

	// Central tiles
	std::vector<std::shared_ptr<Factory>> m_factories;
	std::shared_ptr<Factory> m_centre;

	// Current round info
	Tile::Type m_bonus_type;
	// Used only in SFML implementation
	bool m_centre_taken;
	bool m_picking_stage;
	std::mutex m_thread_running;

	// Member variable for players
	cIndex m_current_player;
	int m_starting_player;
	std::vector<std::shared_ptr<Player>> m_players;
	std::vector<std::shared_ptr<Button>> m_buttons;
	std::vector<std::shared_ptr<sf::Texture>> m_textures;
	std::vector<std::unique_ptr<PlayerVisualizer>> m_player_visualizers;
};

#endif
