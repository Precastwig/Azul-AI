#include "Game.hpp"
#include "players/RandomAI.hpp"
#include <algorithm>
#include <iostream>

Game::Game(int num_players) {
	// Create the bag
	m_bag = std::make_shared<Bag>();
	// Create the players
	for (int i = 0; i < num_players; ++i) {
		std::shared_ptr<Player> randomai = std::make_shared<RandomAI>(all_player_colours[i], m_bag);
		m_players.push_back(randomai);
	}

	// Randomise the starting player
	m_starting_player = rand() % m_players.size();

	// Create the factories
	int num_factories = (num_players * 2) + 1;
	for (int i = 0; i < num_factories; ++i) {
		m_factories.push_back(std::make_shared<Factory>());
	}
	m_centre = std::make_shared<Factory>();
}

Game::~Game() {
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
	int round_number = 1;
	for (Tile bonus_tile : bonus_tile_order) {
		std::cout << "=======================\n";
		std::cout << "Round " << round_number << ":\n";
		std::cout << "Bonus tile colour: " << tile_strings[bonus_tile] << "\n";
		// Fill factories
		fill_factories();
		// First stage
		picking_stage(bonus_tile);
		// Second stage
		placing_stage(bonus_tile);

		round_number++;
	}
	declare_winner();
}

void Game::declare_winner() {
	std::vector<std::shared_ptr<Player>> sorted_players = m_players;
	std::sort(sorted_players.begin(), sorted_players.end(),
	[](std::shared_ptr<Player> a, std::shared_ptr<Player> b) {
        return a->points() > b->points();
    });

	std::cout << "=================\nGame over:\n";
	int pos = 1;
	for (std::shared_ptr<Player> player : sorted_players) {
		std::cout << pos << ". " << player_colour_strings[player->colour()] << "   " << player->points() << " points\n";
		pos++;
	}
}

void Game::fill_factories() {
	for (std::shared_ptr<Factory> factory : m_factories) {
		for (int i = 0; i < 4; ++i) {
			Tile pulled = m_bag->pullTile();
			if (pulled != NONE) {
				factory->place(pulled);
			}
		}
	}
}

bool Game::noTilesLeft() {
	for (std::shared_ptr<Factory> factory : m_factories) {
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
	printFactories();
	while (!noTilesLeft()) {
		std::shared_ptr<Player> player = m_players[player_turn.getIndex()];
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
		printFactories();
	}
}

bool Game::playerNotFinished() {
	for (std::shared_ptr<Player> player : m_players) {
		if (!player->finishedPlacing())
			return true;
	}
	return false;
}

void Game::placing_stage(Tile bonus_tile) {
	std::cout << "\n========================\nPLACING STAGE\n";
	for (std::shared_ptr<Player> player : m_players) {
		player->resetDonePlacing();
	}
	cIndex player_turn(m_starting_player + 1, m_players.size());
	while (playerNotFinished()) {
		std::shared_ptr<Player> current_player = m_players[player_turn.getIndex()];
		PlacingChoice choice = current_player->placeTile(bonus_tile);
		current_player->resolvePlacingChoice(choice, bonus_tile);
		std::cout << current_player->toString();
		player_turn++;
	}
}

void Game::printFactories() {
	std::cout << "\n\nFACTORIES\n-----------------\n";
	for (unsigned int i = 0; i < m_factories.size(); ++i) {
		std::cout << "Factory " << i << ": " << m_factories[i]->toString() << "\n";
	}
	std::cout << "Centre: " << m_centre->toString() << "\n";
}
