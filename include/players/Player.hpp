#ifndef PLAYER
#define PLAYER

#include <memory>
#include "game_elements/Factory.hpp"
#include "game_elements/Board.hpp"
#include "game_elements/Bag.hpp"
#include <utils/Choices.hpp>

class Game;

class Player {
public:
	Player(PlayerColour playercolour, std::shared_ptr<Bag> bag);
	virtual ~Player() = default;

	// Virtuals that need overriding because an in-game choice is needed
	virtual PickingChoice pickTile(
		std::vector<std::shared_ptr<Factory>> factories,
		std::shared_ptr<Factory> centre,
		Tile bonus,
		bool centrePoison
	) = 0;
	virtual PlacingChoice placeTile(Tile bonus) = 0;
	virtual std::vector<std::shared_ptr<Tile>> chooseBonusPieces(std::vector<std::shared_ptr<Tile>> choices, int number) = 0;
	virtual std::vector<std::shared_ptr<Tile>> discardDownToFour() = 0;

	// Other helpers
	virtual bool isAI() = 0;
	void pickBonusPieces(int number);
	bool hasTiles();
	int numTiles();
	void addPoints(int points);
	void minusPoisonPoints();
	int points() {
		return m_points;
	};
	PlayerColour colour() {
		return m_col;
	};
	Board* getBoardPtr() {
		return &m_board;
	}

	// Resolvers
	void resolvePickingChoice(PickingChoice& choice, TileType bonus, std::shared_ptr<Factory> centre, bool& centre_taken, int& startingPlayer, int currentPlayerIndex);
	void resolvePlacingChoice(PlacingChoice& choice, TileType bonus);

	bool finishedPlacing() {
		return m_done_placing;
	};
	void resetDonePlacing() {
		m_done_placing = false;
		m_discarded = false;
	};

	// A weird cmd line only thing to look like it's thinking
	void commandLineWait();

	std::string toShortString();
	std::string toStringNoBoard();
	std::string toString();
protected:
	std::vector<std::shared_ptr<Location>> getLocationsFromChoiceList(std::vector<PlacingChoice> choices);
	std::vector<PlacingChoice> filterChoicesFromLocation(std::vector<PlacingChoice> choices, std::shared_ptr<Location> location);
	std::vector<PickingChoice> getAllPickingChoices(
		std::vector<std::shared_ptr<Factory>> factories,
		std::shared_ptr<Factory> centre,
		TileType bonus
	);
	std::vector<PlacingChoice> getAllowedPlacingChoices(Tile bonus);
	void createAllVariationsOfChoice(
		PlacingChoice base_choice, std::vector<PlacingChoice>& return_list,
		int max_bonus,
		int max_relevent_colour
	);

	int howManyColourStored(TileType t, std::vector<std::shared_ptr<Tile>> stored);

	// Flag for when the player hass passed
	bool m_done_placing;
	// Flag for when the player has discarded down to 4 tiles remaining
	bool m_discarded;
	// The board
	Board m_board;
	// The tiles taken to be placed on the board
	std::vector<std::shared_ptr<Tile>> m_stored_tiles;
	PlayerColour m_col;
	std::shared_ptr<Bag> m_bag;
private:
	int m_points;
};

#endif
