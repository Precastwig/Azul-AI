#ifndef GAME
#define GAME

#include <vector>
#include "players/Player.hpp"
#include "game_elements/Bag.hpp"
#include "game_elements/Factory.hpp"

class Game {
public:
	Game(int num_players);
	~Game();

	void play();

	void takeRewardTiles(std::vector<Tile> tiles);
	std::vector<Tile> rewardTiles();
private:
	void printFactories();

	// Turn steps
	void fill_factories();
	void picking_stage(Tile bonus_tile);
	void placing_stage();

	// Helper functions
	bool noTilesLeft();
	bool playerHasTiles();

	// Reward tiles
	std::vector<Tile> m_reward_tiles;

	// Tile sources
	Bag m_tile_bag;

	// Central tiles
	std::vector<Factory*> m_factories;
	Factory* m_centre;

	// Member variable for players
	int m_starting_player;
	std::vector<Player*> m_players;
};

#endif
