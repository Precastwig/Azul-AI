#ifndef PLAYER
#define PLAYER

#include "game_elements/Factory.hpp"
#include "game_elements/Board.hpp"

class Game;

struct PickingChoice {
	Factory* factory;
	Tile tile;
	bool with_bonus;
};

class Player {
public:
	Player(PlayerColour colour, Game* game);
	virtual ~Player() = default;

	// Virtuals that need overriding either by AI or human player variant
	virtual PickingChoice pickTile(
		std::vector<Factory*> factories,
		Factory* centre,
		Tile bonus,
		bool centrePoison
	) = 0;
	virtual void placeTile() = 0;
	virtual void pickBonusPieces(int number) = 0;

	// Other helpers
	bool hasTiles();
	void addPoints(int points);
	void minusPoisonPoints();
	void resolvePickingChoice(PickingChoice choice, Tile bonus, Factory* centre);
protected:
	std::vector<PickingChoice> getAllPickingChoices(
		std::vector<Factory*> factories,
		Factory* centre,
		Tile bonus
	);
	std::vector<PlacingChoice> getAllowedPlacingChoices();
	Board m_board;
private:

	const Game* m_game;
	int m_num_of_each_tile[7] = {0,0,0,0,0,0,0};
	PlayerColour m_col;
	int m_points;
};

#endif
