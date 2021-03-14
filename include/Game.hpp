#ifndef GAME
#define GAME

#include <vector>
#include <memory>
#include "players/Player.hpp"
#include "game_elements/Bag.hpp"
#include "game_elements/Factory.hpp"

class Game {
public:
	// Constructor/Destructor
	Game(int num_players);
	~Game();

	// Initiates the play sequence
	void play();
private:
	// Prints the factories to stdout
	void printFactories();
	// Print some info to stdout declaring the winner of the game
	void declare_winner();

	//-------------------------------------------------------------------
	// Turn step functions

	// Pulls tiles from the bag and places on the factories until each
	// factory contains 4, or the bag runs out
	void fill_factories();
	// Players take turns picking tiles from factories until all are empty
	void picking_stage(Tile bonus_tile);
	// Players take turns placing tiles on their personal boards until they choose to stop (or can't place any more tiles)
	void placing_stage(Tile bonus_tile);

	// Helper functions

	// Check if all factories are empty
	bool noTilesLeft();
	// Check that there is a player that still wishes to place tiles
	bool playerNotFinished();

	//-------------------------------------------------------------------
	// Member variables

	// Tile sources
	std::shared_ptr<Bag> m_bag;

	// Central tiles
	std::vector<std::shared_ptr<Factory>> m_factories;
	std::shared_ptr<Factory> m_centre;

	// Member variable for players
	int m_starting_player;
	std::vector<std::shared_ptr<Player>> m_players;
};

#endif
