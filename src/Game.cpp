#include "Game.hpp"
#include "game_elements/Button.hpp"
#include "players/RandomAI.hpp"
#include "players/HumanCommandLine.hpp"
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <thread>

Game::Game(sf::Vector2f size) : m_debug_switchstage("Switch stage"), m_centre_taken(false), m_bonus_type(m_bonus_tile_order[0]), m_picking_stage(true) {
	// Create the bag
	m_bag = std::make_shared<Bag>();
	// Create the players, for now one randomAI and one humancmd

	std::shared_ptr<Player> player1 = std::make_shared<HumanCommandLine>(PlayerColour::all_colours()[0], m_bag);
	std::shared_ptr<Player> player2 = std::make_shared<RandomAI>(PlayerColour::all_colours()[1], m_bag);
	m_players.push_back(player1);
	m_players.push_back(player2);

	for (std::shared_ptr<Player> p : m_players) {
		m_boards.push_back(p->getBoardPtr());
	}

	// Randomise the starting player
	m_starting_player = rand() % m_players.size();
	m_current_player = cIndex(m_starting_player + 1, m_players.size());

	sf::Vector2f playerSize(size.x / 8,size.y / 2);
	std::vector<sf::Vector2f> positions;
	positions.push_back(sf::Vector2f(0,0));
	positions.push_back(sf::Vector2f(size.x - playerSize.x,0));
	positions.push_back(sf::Vector2f(0,size.y - playerSize.y));
	positions.push_back(sf::Vector2f(size.x - playerSize.x, size.y - playerSize.y));

	for (size_t i = 0; i < m_players.size(); ++i) {
		auto ptr = std::make_unique<PlayerVisualizer>(m_players[i], 
		positions[i], playerSize);
		m_player_visualizers.push_back(std::move(ptr));
	}

	// Create the factories
	int num_factories = (m_players.size() * 2) + 1;
	double angle = 0.0;
	sf::Vector2f middle(size.x / 2,size.y / 2);
	for (int i = 0; i < num_factories; ++i) {
		sf::Vector2f factoryPos = Factory::calculateNewPos(middle, 250, angle);
		m_factories.push_back(std::make_shared<Factory>(i, factoryPos, 60));
		angle += (2 * M_PI / num_factories);
	}
	m_centre = std::make_shared<Factory>(num_factories, middle, 60);
	m_debug_switchstage.setPosition(positions[2]);
	m_debug_switchstage.m_callback = [&]() {
		this->flipStage();
	};
	// Alternately could use bind?
	//	std::bind(&Game::flipStage, std::ref(*this));
}

Game::~Game() {
}

void Game::onClick(int xPos, int yPos) {
	// The user should only be able to interact if the AI isn't currently running
	if (m_thread_running.try_lock()) {
		// Find the object that we've clicked on and call its onClick event
		for (std::shared_ptr<Factory> factory : m_factories) {
			if (factory->contains(xPos, yPos)) {
				factory->onClick(xPos, yPos, *this);
			}
		}
		if (m_centre->contains(xPos, yPos)) {
			m_centre->onClick(xPos, yPos, *this);
		}
		m_debug_switchstage.onClick(xPos, yPos);
		m_thread_running.unlock();
	}
}

void Game::draw(RenderTarget &target, RenderStates states) const {
	if (m_picking_stage) {
		m_centre->draw(target, states);
		for (std::shared_ptr<Factory> factory : m_factories) {
			factory->draw(target, states);
		}
	} else {
		// Placing stage
		// Only show the placing players board for now?
		m_boards[m_current_player.getIndex()]->draw(target, states);
		// for (Board* b : m_boards) {
		// 	b->draw(target, states);
		// }
	}
	for (size_t i = 0; i < m_player_visualizers.size(); ++i) {
		m_player_visualizers[i]->draw(target, states);
	}

	m_debug_switchstage.draw(target, states);
}

void performAIActionWorker(Game* game, std::shared_ptr<Player> player) {
	// Perform the action then move the player forward
	game->performAIAction(player);
}

void Game::performAIAction(std::shared_ptr<Player> player) {
	if (m_picking_stage) {
		PickingChoice choice = player->pickTile(
			m_factories,
			m_centre,
			m_bonus_type,
			!m_centre_taken
		);
		pick_tile(choice);
	} else { 
		PlacingChoice choice = player->placeTile(m_bonus_type);
		player->resolvePlacingChoice(choice, m_bonus_type);
		m_current_player++;
	}
	m_thread_running.unlock();
}

void Game::performAIActions() {
	std::shared_ptr<Player> player = m_players[m_current_player.getIndex()];
	// Start the AI actions in another thread as to not lock up the
	// graphics
	if (player->isAI() && m_thread_running.try_lock()) {
		std::thread aiThread(performAIActionWorker, this, player);
		aiThread.detach();
	}
}

void Game::play() {
	int round_number = 1;
	for (Tile::Type bonus_tile : m_bonus_tile_order) {
		m_bonus_type = bonus_tile;
		std::cout << "=======================\n";
		std::cout << "Round " << round_number << ":\n";
		std::cout << "Bonus tile colour: " << Tile::toString(m_bonus_type) << "\n";
		// Fill factories
		fill_factories();
		// First stage
		m_picking_stage = true;
		picking_stage();
		m_picking_stage = false;
		// Second stage
		placing_stage();

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
		m_bag->fillFactory(factory);
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

void Game::picking_stage() {
	cIndex player_turn(m_starting_player + 1, m_players.size());
	// Double negative, I won't apologise
	bool centreTaken = false;
	printFactories();
	while (!noTilesLeft()) {
		std::shared_ptr<Player> player = m_players[player_turn.getIndex()];
		PickingChoice choice = player->pickTile(
			m_factories,
			m_centre,
			m_bonus_type,
			!centreTaken
		);
		player->resolvePickingChoice(choice, m_bonus_type, m_centre, m_centre_taken, m_starting_player, player_turn.getIndex());
		player_turn++;
		printFactories();
	}
}

void Game::pick_tile(PickingChoice& picked) {
	std::shared_ptr<Player> player = m_players[m_current_player.getIndex()];
	std::string withBonus = (picked.with_bonus) ? "with a bonus tile" : "";
	g_logger.log(Logger::INFO, player->toShortString() + " has picked " + std::to_string(picked.factory->numberOf(picked.tile_colour)) + " " + Tile::toString(picked.tile_colour) + " " + withBonus);

	player->resolvePickingChoice(picked, m_bonus_type, m_centre, m_centre_taken, m_starting_player, m_current_player.getIndex());
	if (noTilesLeft()) {
		m_current_player = cIndex(m_starting_player + 1, m_players.size());
		m_picking_stage = false;
	} else {
		m_current_player++;
	}
	// Give the player visualizers a chance to show new info
	for (size_t i = 0; i < m_player_visualizers.size(); ++i) {
		m_player_visualizers[i]->update(m_players[m_current_player.getIndex()]);
	}
}

bool Game::playerNotFinished() {
	for (std::shared_ptr<Player> player : m_players) {
		if (!player->finishedPlacing())
			return true;
	}
	return false;
}

Tile::Type Game::getBonus() {
	return m_bonus_type;
}

void Game::placing_stage() {
	std::cout << "\n========================\nPLACING STAGE\n";
	for (std::shared_ptr<Player> player : m_players) {
		player->resetDonePlacing();
	}
	cIndex player_turn(m_starting_player + 1, m_players.size());
	while (playerNotFinished()) {
		std::shared_ptr<Player> current_player = m_players[player_turn.getIndex()];
		PlacingChoice choice = current_player->placeTile(m_bonus_type);
		current_player->resolvePlacingChoice(choice, m_bonus_type);
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
