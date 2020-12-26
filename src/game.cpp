#include "game.hpp"
#include "randomAI.hpp"
#include <algorithm>
#include <iostream>

Game::Game(int num_players) : m_tile_bag() {
	// Create the players
	for (int i = 0; i < num_players; ++i) {
		Player* randomai = new RandomAI(all_player_colours[i], this);
		m_players.push_back(randomai);
	}

	// Randomise the starting player
	m_starting_player = rand() % m_players.size();

	// Create the factories
	int num_factories = (num_players * 2) + 1;
	for (int i = 0; i < num_factories; ++i) {
		m_factories.push_back(new Factory());
	}
	m_centre = new Factory();

	// Fill the reward tiles
	for (int i = 0; i < 10; ++i) {
		Tile pulled = m_tile_bag.pullTile();
		m_reward_tiles.push_back(pulled);
	}
}

Game::~Game() {
	for (Player* p : m_players) {
		delete p;
	}

	for (Factory* f : m_factories) {
		delete f;
	}

	delete m_centre;
}

void Game::play() {
	// Six rounds
	const std::vector<Tile> bonus_tile_order = {
		PURPLE,
		GREEN,
		ORANGE,
		YELLOW,
		BLUE,
		RED
	};
	for (Tile bonus_tile : bonus_tile_order) {
		// Fill factories
		fill_factories();
		// First stage
		picking_stage(bonus_tile);
		// Second stage
		placing_stage();
	}
}

void Game::fill_factories() {
	for (Factory* factory : m_factories) {
		for (int i = 0; i < 4; ++i) {
			Tile pulled = m_tile_bag.pullTile();
			if (pulled != NONE) {
				factory->place(pulled);
			}
		}
	}
}

bool Game::noTilesLeft() {
	for (Factory* factory : m_factories) {
		if (!factory->isEmpty()) {
			return false;
		}
	}
	return m_centre->isEmpty();
}

void Game::picking_stage(Tile bonus_tile) {
	cIndex player_turn(m_starting_player + 1, m_players.size());
	// Double negative, I won't apologise
	bool centreTaken = false;
	while (!noTilesLeft()) {
		printFactories();
		Player* player = m_players[player_turn.getIndex()];
		PickingChoice choice = player->pickTile(
			m_factories,
			m_centre,
			bonus_tile,
			!centreTaken
		);
		if (!centreTaken && choice.factory == m_centre) {
			// Someone has taken the centre!
			centreTaken = true;
			player->minusPoisonPoints();
			m_starting_player = player_turn.getIndex();
		}
		player->resolvePickingChoice(choice, bonus_tile, m_centre);
		player_turn++;
	}
}

bool Game::playerHasTiles() {
	for (Player* player : m_players) {
		if (player->hasTiles())
			return true;
	}
	return false;
}

std::vector<Tile> Game::rewardTiles() {
	return m_reward_tiles;
}

void Game::takeRewardTiles(std::vector<Tile> tiles) {
	for (Tile tile : tiles) {
		auto tile_iter = std::find(m_reward_tiles.begin(), m_reward_tiles.end(), tile);
		if (tile_iter != m_reward_tiles.end()) {
			// We find the tile, remove from the reward tiles and replace from bag
			m_reward_tiles.erase(tile_iter);
			Tile replacement = m_tile_bag.pullTile();
			if (replacement != NONE) {
				m_reward_tiles.push_back(replacement);
			}
		} else {
			// if it's not in there something's gone wrong!
			// Error?
		}
	}
}

void Game::placing_stage() {
	cIndex player_turn(m_starting_player + 1, m_players.size());
	while (playerHasTiles()) {
		m_players[player_turn.getIndex()]->placeTile();
		player_turn++;
	}
}

void Game::printFactories() {
	std::cout << "\n\n\nFACTORIES\n-----------------\n";
	for (unsigned int i = 0; i < m_factories.size(); ++i) {
		std::cout << "Factory " << i << ": " << m_factories[i]->toString() << "\n";
	}
	std::cout << "Centre: " << m_centre->toString() << "\n";
}
