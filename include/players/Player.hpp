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
	Player(PlayerColour playercolour, std::shared_ptr<Bag> bag, sf::Vector2f boardpos);
	virtual ~Player() = default;

	// Virtuals that need overriding because an in-game choice is needed
	virtual PickingChoice pickTile(
		std::vector<std::shared_ptr<Factory>> factories,
		std::shared_ptr<Factory> centre,
		Tile bonus,
		bool centrePoison
	) = 0;
	virtual PlacingChoice placeTile(Tile bonus) = 0;
	virtual std::vector<std::shared_ptr<Tile>> chooseBonusPieces(std::vector<std::shared_ptr<Tile>> choices) = 0;
	virtual void discardDownToFour() = 0;
	// Simple differentiation for AI player classes
	virtual bool isAI() = 0;

	// Other helpers

	// Tile related helpers
	std::vector<std::shared_ptr<Tile>> getTiles() {return m_stored_tiles;};
	void discardTile(std::shared_ptr<Tile> tile) {
		m_bag->toBin({tile});
		m_stored_tiles.erase(std::remove(m_stored_tiles.begin(), m_stored_tiles.end(), tile));
	}
	void addTile(std::shared_ptr<Tile> tile) {
		m_stored_tiles.push_back(tile);
		sortTiles();	
	}
	bool hasTiles();
	int numTiles();
	void sortTiles();
	void highlightCostTiles(const PlacingChoice& choice, TileType bonus);

	void setBonusToPick(int num) {m_bonus_to_choose = num;}
	int getBonusToPick() {return m_bonus_to_choose;}
	void pickBonusPieces();

	// Point related helpers
	void addPoints(int points);
	int getPoisonPoints();
	void minusPoisonPoints();
	int points() {
		return m_points;
	};

	// Generic helpers
	std::vector<PlacingChoice> getAllowedPlacingChoices(Tile bonus);
	PlayerColour colour() {
		return m_col;
	};
	Board* getBoardPtr() {
		return &m_board;
	}

	// Resolvers
	void resolvePickingChoice(PickingChoice& choice, TileType bonus, std::shared_ptr<Factory> centre);
	void resolvePlacingChoice(PlacingChoice& choice, TileType bonus);

	// Turn related helpers
	void pass();
	bool finishedPlacing() {
		return m_done_placing;
	};
	void resetDonePlacing() {
		m_done_placing = false;
		m_discarded = false;
	};

	// A weird thing to look like it's thinking
	void commandLineWait();

	// Various tostring functions
	virtual std::string playerTypeString() = 0;
	std::string colourString() {return m_col.toString();}
	std::string toShortString();
	std::string toStringNoBoard();
	std::string toString();
protected:
	std::vector<std::shared_ptr<Location>> getLocationsFromChoiceList(std::vector<PlacingChoice> choices);
	std::vector<PickingChoice> getAllPickingChoices(
		std::vector<std::shared_ptr<Factory>> factories,
		std::shared_ptr<Factory> centre,
		TileType bonus
	);
	void createAllVariationsOfChoice(
		PlacingChoice base_choice, std::vector<PlacingChoice>& return_list,
		int max_bonus,
		int max_relevent_colour
	);

	int howManyColourStored(TileType t, std::vector<std::shared_ptr<Tile>> stored);

	int m_bonus_to_choose;
	// Flag for when the player hass passed
	bool m_done_placing;
	// Flag for when the player has discarded down to 4 tiles remaining
	bool m_discarded;
	// The board
	Board m_board;
	// The tiles taken to be placed on the board
	int m_stored_from_previous_round;
	PlayerColour m_col;
	std::shared_ptr<Bag> m_bag;
private:
	std::vector<std::shared_ptr<Tile>> m_stored_tiles;
	int m_points;
};

#endif
