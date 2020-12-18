#ifndef GAME
#define GAME

#include <vector>
#include "player.hpp"

class Game {
public:
	Game(int num_players);

	void play();

	void takeRewardTiles(std::vector<Tile> tiles);
	std::vector<Tile> rewardTiles();
private:
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
