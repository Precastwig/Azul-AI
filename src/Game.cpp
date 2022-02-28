#include "Game.hpp"
#include "game_elements/RoundVisualizer.hpp"
#include "players/RandomAI.hpp"
#include "players/Human.hpp"
#include "utils/helper_enums.hpp"
#include <ui_elements/Button.hpp>
#include <players/PlayerInfo.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <thread>

GameState g_visual_state;
PlayerInfo g_player_info;

Game::Game(sf::Vector2f size) : m_debug_switchstage("Switch stage"), m_centre_taken(false), m_round_num(0) {
	// Create the bag
	m_bag = std::make_shared<Bag>();
	g_visual_state = GameState::PICKING;

	sf::Vector2f playerVisualSize(size.x / 8,size.y / 2);
	std::vector<sf::Vector2f> playerVisualpositions;
	playerVisualpositions.push_back(sf::Vector2f(0,0));
	playerVisualpositions.push_back(sf::Vector2f(size.x - playerVisualSize.x,0));
	playerVisualpositions.push_back(sf::Vector2f(0,size.y - playerVisualSize.y));
	playerVisualpositions.push_back(sf::Vector2f(size.x - playerVisualSize.x, size.y - playerVisualSize.y));
	// Create the players, for now one randomAI and one human
	std::shared_ptr<Player> player1 = std::make_shared<Human>(
		PlayerColour::all_colours()[0], 
		m_bag, 
		sf::Vector2f(
			playerVisualSize.x + 300,
			300
		)
	);
	std::shared_ptr<Player> player2 = std::make_shared<RandomAI>(
		PlayerColour::all_colours()[1], 
		m_bag,
		sf::Vector2f(
			playerVisualpositions[1].x - 300,
			300
		)
	);
	g_player_info.addPlayer(player1);
	g_player_info.addPlayer(player2);

	for (std::shared_ptr<Player> p : g_player_info.getPlayers()) {
		m_boards.push_back(p->getBoardPtr());
	}

	// Randomise the starting player
	g_player_info.initialiseIndicies();

	for (size_t i = 0; i < g_player_info.numPlayers(); ++i) {
		auto ptr = std::make_unique<PlayerVisualizer>(
			g_player_info.getPlayer(i), 
			playerVisualpositions[i], 
			playerVisualSize 
		);
		m_player_visualizers.push_back(std::move(ptr));
	}

	// Create the factories
	int num_factories = (g_player_info.numPlayers() * 2) + 1;
	double angle = 0.0;
	sf::Vector2f middle(size.x / 2,size.y / 2);
	for (int i = 0; i < num_factories; ++i) {
		sf::Vector2f factoryPos = Factory::calculateNewPos(middle, 250, angle);
		m_factories.push_back(std::make_shared<Factory>(i, factoryPos, 60));
		angle += (2 * M_PI / num_factories);
	}
	m_centre = std::make_shared<Factory>(num_factories, middle, 60);
	m_debug_switchstage.setPosition(playerVisualpositions[2]);
	m_debug_switchstage.m_callback = []() {
		if (g_visual_state == GameState::PICKING) {
			g_visual_state = GameState::PLACING;
		} else if (g_visual_state == GameState::PLACING) {
			g_visual_state = GameState::PICKING;
		}
	};
	// Alternately could use bind?
	//	std::bind(&Game::flipStage, std::ref(*this));

	m_round_visualizer = std::make_unique<RoundVisualizer>(m_bonus_tile_order, sf::Vector2f(size.x/2, size.y *9 / 10));
}

Game::~Game() {
}

void Game::onClick(int xPos, int yPos) {
	// The user should only be able to interact if the AI isn't currently running
	if (m_thread_running.try_lock()) {
		// Find the object that we've clicked on and call its onClick event
		if (g_visual_state == GameState::PICKING) {
			for (std::shared_ptr<Factory> factory : m_factories) {
				factory->onClick(xPos, yPos, *this);
			}
			m_centre->onClick(xPos, yPos, *this);
		} else {
			std::shared_ptr<Player> currPlayer = g_player_info.getCurrentPlayer();
			currPlayer->getBoardPtr()->onClick(xPos, yPos, *this);
			
			for (auto& vp : m_player_visualizers) {
				vp->onClick(xPos, yPos);
			}
		}
		m_debug_switchstage.onClick(xPos, yPos);
		m_thread_running.unlock();
	}
}

void Game::onHover(int xPos, int yPos) {
	if (g_visual_state == GameState::PICKING) {
		for (std::shared_ptr<Factory> factory : m_factories) {
			factory->onHover(xPos, yPos, getBonus());
		}
		m_centre->onHover(xPos, yPos, getBonus());
	} else {
		std::shared_ptr<Player> currPlayer = g_player_info.getCurrentPlayer();
		currPlayer->getBoardPtr()->onHover(xPos, yPos);
		for (auto& pv : m_player_visualizers) {
			pv->onHover(xPos, yPos);
		}
	}
}

void Game::draw(RenderTarget &target, RenderStates states) const {
	if (g_visual_state == GameState::PICKING) {
		m_centre->draw(target, states);
		for (std::shared_ptr<Factory> factory : m_factories) {
			factory->draw(target, states);
		}
	} else if (g_visual_state == GameState::PLACING || g_visual_state == GameState::DISCARDING) {
		// Placing stage
		for (Board* b : getVisualisedBoards()) {
			if (b) {
				b->draw(target, states);
			}
		}
		//m_boards[m_current_player.getIndex()]->draw(target, states);
	}
	for (size_t i = 0; i < m_player_visualizers.size(); ++i) {
		m_player_visualizers[i]->draw(target, states);
	}
	m_round_visualizer->draw(target, states);
	m_debug_switchstage.draw(target, states);
}

std::vector<Board*> Game::getVisualisedBoards() const {
	return m_boards;
}

void performAIActionWorker(Game* game, std::shared_ptr<Player> player) {
	// Perform the action then move the player forward
	game->performAIAction(player);
}

void Game::performAIAction(std::shared_ptr<Player> player) {
	if (g_visual_state == GameState::PICKING) {
		PickingChoice choice = player->pickTile(
			m_factories,
			m_centre,
			getBonus(),
			!m_centre_taken
		);
		pick_tile(choice);
	} else if (g_visual_state == GameState::PLACING) { 
		PlacingChoice choice = player->placeTile(getBonus());
		place_tile(choice);
	} else if (g_visual_state == GameState::DISCARDING) {
		player->discardDownToFour();
	}
	m_thread_running.unlock();
}

void Game::performAIActions() {
	std::shared_ptr<Player> player = g_player_info.getCurrentPlayer();
	// Start the AI actions in another thread as to not lock up the
	// graphics
	if (player->isAI() && m_thread_running.try_lock()) {
		std::thread aiThread(performAIActionWorker, this, player);
		aiThread.detach();
	}
}

void Game::play() {
	int round_number = 1;
	for (size_t i = 0; i < m_bonus_tile_order.size(); ++i) {
		m_round_num = i;
		std::cout << "=======================\n";
		std::cout << "Round " << round_number << ":\n";
		std::cout << "Bonus tile colour: " << Tile::toString(getBonus()) << "\n";
		// Fill factories
		fill_factories();
		// First stage
		picking_stage();
		// Second stage
		placing_stage();

		round_number++;
	}
	declare_winner();
}

void Game::declare_winner() {
	std::vector<std::shared_ptr<Player>> sorted_players = g_player_info.getPlayers();
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
	// Double negative, I won't apologise
	bool centreTaken = false;
	printFactories();
	while (!noTilesLeft()) {
		std::shared_ptr<Player> player = g_player_info.getCurrentPlayer();
		PickingChoice choice = player->pickTile(
			m_factories,
			m_centre,
			getBonus(),
			!centreTaken
		);
		player->resolvePickingChoice(choice, getBonus(), m_centre, m_centre_taken);
		g_player_info.nextTurn();
		printFactories();
	}
}

void Game::pick_tile(PickingChoice& picked) {
	std::shared_ptr<Player> player = g_player_info.getCurrentPlayer();
	std::string withBonus = (picked.with_bonus) ? "with a bonus tile" : "";
	g_logger.log(Logger::INFO, player->toShortString() + " has picked " + std::to_string(picked.factory->numberOf(picked.tile_colour)) + " " + Tile::toString(picked.tile_colour) + " " + withBonus);

	player->resolvePickingChoice(picked, getBonus(), m_centre, m_centre_taken);
	if (noTilesLeft()) {
		switchToPlacingStage();
	} else {
		g_player_info.nextTurn();
	}
	// Give the player visualizers a chance to show new info
	updatePlayerVisualizers();
}

void Game::updatePlayerVisualizers() {
	for (size_t i = 0; i < m_player_visualizers.size(); ++i) {
		m_player_visualizers[i]->update();
	}
}

bool Game::playerNotFinished() {
	for (std::shared_ptr<Player> player : g_player_info.getPlayers()) {
		if (!player->finishedPlacing())
			return true;
	}
	return false;
}

TileType Game::getBonus() {
	return m_bonus_tile_order[m_round_num];
}

void Game::switchToPlacingStage() {
	g_visual_state = GameState::PLACING;
	for (std::shared_ptr<Player> player : g_player_info.getPlayers()) {
		player->resetDonePlacing();
	}
	updatePlayerVisualizers();
}

void Game::switchToPickingStage() {
	g_visual_state = GameState::PICKING;
	if (m_round_num < m_bonus_tile_order.size()) {
		m_round_num++;
		m_round_visualizer->nextround();
		fill_factories();
		updatePlayerVisualizers();
	} else {
		// End the game
		// Somehow
	}
}

void Game::place_tile(PlacingChoice& placed) {
	// The m_current_player var should be correct at this point
	// But lets check anyway
	std::shared_ptr<Player> player = g_player_info.getCurrentPlayer();
	if (player->finishedPlacing() || placed.star == nullptr) {
		g_logger.log(Logger::ERROR, "Place_tile called when current player has finished placing");
		g_player_info.nextTurn();
		if (!playerNotFinished()) {
			switchToPickingStage();
		}
		return;
	}

	player->resolvePlacingChoice(placed, getBonus());

	if (playerNotFinished()) {
		// This could be better? but I can't think of how right now
		bool lookingForNextPlayer = true;
		while (lookingForNextPlayer) {
			g_player_info.nextTurn();
			if (!g_player_info.getCurrentPlayer()->finishedPlacing()) {
				lookingForNextPlayer = false;
			}
		}
	} else {
		switchToPickingStage();
	}
	updatePlayerVisualizers();
}

void Game::placing_stage() {
	std::cout << "\n========================\nPLACING STAGE\n";
	for (std::shared_ptr<Player> player : g_player_info.getPlayers()) {
		player->resetDonePlacing();
	}
	while (playerNotFinished()) {
		std::shared_ptr<Player> current_player = g_player_info.getCurrentPlayer();
		PlacingChoice choice = current_player->placeTile(getBonus());
		current_player->resolvePlacingChoice(choice, getBonus());
		std::cout << current_player->toString();
		g_player_info.nextTurn();
	}
}

void Game::printFactories() {
	std::cout << "\n\nFACTORIES\n-----------------\n";
	for (unsigned int i = 0; i < m_factories.size(); ++i) {
		std::cout << "Factory " << i << ": " << m_factories[i]->toString() << "\n";
	}
	std::cout << "Centre: " << m_centre->toString() << "\n";
}
