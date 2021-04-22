#include "Game.hpp"
#include "players/RandomAI.hpp"
#include "players/HumanCommandLine.hpp"
#include <algorithm>
#include <iostream>

Game::Game() {
	// Create the bag
	m_bag = std::make_shared<Bag>();
	// Create the players, for now one randomAI and one humancmd

	std::shared_ptr<Player> randomai = std::make_shared<RandomAI>(PlayerColour::all_colours()[0], m_bag);
	std::shared_ptr<Player> player = std::make_shared<HumanCommandLine>(PlayerColour::all_colours()[1], m_bag);
	m_players.push_back(randomai);
	m_players.push_back(player);

	// Randomise the starting player
	m_starting_player = rand() % m_players.size();

	// Create the factories
	int num_factories = (m_players.size() * 2) + 1;
	for (int i = 0; i < num_factories; ++i) {
		m_factories.push_back(std::make_shared<Factory>(i));
	}
	m_centre = std::make_shared<Factory>(num_factories);
}

Game::~Game() {
}

void Game::draw (RenderTarget &target, RenderStates states) const {
	m_centre->draw(target, states);
	for (std::shared_ptr<Factory> factory : m_factories) {
		factory->draw(target, states);
	}
}

void Game::play() {
	// Six rounds
	const std::vector<Tile> bonus_tile_order = {
		Tile(Tile::PURPLE),
		Tile(Tile::GREEN),
		Tile(Tile::ORANGE),
		Tile(Tile::YELLOW),
		Tile(Tile::BLUE),
		Tile(Tile::RED)
	};
	int round_number = 1;
	for (Tile bonus_tile : bonus_tile_order) {
		std::cout << "=======================\n";
		std::cout << "Round " << round_number << ":\n";
		std::cout << "Bonus tile colour: " << bonus_tile.toString() << "\n";
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
		std::cout << pos << ". " << player->colour().toString() << "   " << player->points() << " points\n";
		pos++;
	}
}

void Game::fill_factories() {
	for (std::shared_ptr<Factory> factory : m_factories) {
		for (int i = 0; i < 4; ++i) {
			Tile pulled = m_bag->pullTile();
			if (pulled != Tile::NONE) {
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
