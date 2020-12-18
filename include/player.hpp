#ifndef PLAYER
#define PLAYER

#include "helper_classes.hpp"
#include "board.hpp"

class Game;

class Player {
public:
	Player(PlayerColour colour, Game* game);

	// Virtuals that need overriding either by AI or human player variant
	virtual void pickTile(
		std::vector<Factory*> factories,
		Factory* centre,
		Tile bonus,
		bool& centrePoison
	) = 0;
	virtual void placeTile() = 0;
	virtual void pickBonusPieces(int number) = 0;

	// Other helpers
	bool hasTiles();
	void addPoints(int points);
private:
	const Game* m_game;
	int m_num_of_each_tile[7] = {0,0,0,0,0,0,0};
	PlayerColour m_col;
	Board m_board;
	int m_points;
};

#endif
