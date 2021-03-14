#ifndef PLAYER
#define PLAYER

#include <memory>
#include "game_elements/Factory.hpp"
#include "game_elements/Board.hpp"
#include "game_elements/Bag.hpp"

class Game;

struct PickingChoice {
	std::shared_ptr<Factory> factory;
	Tile tile;
	bool with_bonus;
};

class Player {
public:
	Player(PlayerColour colour, std::shared_ptr<Bag> piecestores);
	virtual ~Player() = default;

	// Virtuals that need overriding either by AI or human player variant
	virtual PickingChoice pickTile(
		std::vector<std::shared_ptr<Factory>> factories,
		std::shared_ptr<Factory> centre,
		Tile bonus,
		bool centrePoison
	) = 0;
	virtual PlacingChoice placeTile(Tile bonus) = 0;
	virtual std::vector<Tile> chooseBonusPieces(std::vector<Tile> choices, int number) = 0;

	// Other helpers
	void pickBonusPieces(int number);
	bool hasTiles();
	void addPoints(int points);
	void minusPoisonPoints();
	int points() {
		return m_points;
	};
	PlayerColour const colour() const {
		return m_col;
	};

	// Resolvers
	void resolvePickingChoice(PickingChoice choice, Tile bonus, std::shared_ptr<Factory> centre);
	void resolvePlacingChoice(PlacingChoice choice, Tile bonus);

	bool finishedPlacing() {
		return m_done_placing;
	};
	void resetDonePlacing() {
		m_done_placing = false;
	};

	std::string toString();
protected:
	std::vector<PickingChoice> getAllPickingChoices(
		std::vector<std::shared_ptr<Factory>> factories,
		std::shared_ptr<Factory> centre,
		Tile bonus
	);
	std::vector<PlacingChoice> getAllowedPlacingChoices(Tile bonus);
	void createAllVariationsOfChoice(
		PlacingChoice base_choice, std::vector<PlacingChoice>& return_list,
		int max_bonus,
		int max_relevent_colour
	);
	bool m_done_placing;
	Board m_board;
private:

	std::shared_ptr<Bag> m_piece_stores;
	std::vector<int> m_num_of_each_tile = {0,0,0,0,0,0};
	const PlayerColour m_col;
	int m_points;
};

#endif
