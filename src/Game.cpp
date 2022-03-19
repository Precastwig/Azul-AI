#include "Game.hpp"
#include "game_elements/RoundVisualizer.hpp"
#include "players/RandomAI.hpp"
#include "players/Human.hpp"
#include "utils/helper_enums.hpp"
#include <SFML/Graphics/Drawable.hpp>
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
extern MenuState g_menu_state;

Game::Game(std::vector<PlayerType> players, sf::Vector2f size) : m_screen_size(size), m_debug_switchstage("Switch stage"), m_centre_taken(false), m_round_num(0), m_finish_screen(nullptr) {
	g_player_info.wipeInfo();
	g_visual_state = GameState::PICKING;
	// Create the bag
	sf::Vector2f middle(size.x / 2,size.y / 2);
	sf::Vector2f playerVisualSize(size.x / 8,size.y / 2);
	float reward_size_x = size.x - (playerVisualSize.x * 4);
	// a square, for now
	sf::Vector2f reward_size(reward_size_x, std::min(reward_size_x, size.y));
	m_bag = std::make_shared<Bag>(reward_size, middle);
	g_visual_state = GameState::PICKING;

	std::vector<sf::Vector2f> playerVisualpositions;
	playerVisualpositions.push_back(sf::Vector2f(0,0));
	playerVisualpositions.push_back(sf::Vector2f(size.x - playerVisualSize.x,0));
	playerVisualpositions.push_back(sf::Vector2f(0,size.y - playerVisualSize.y));
	playerVisualpositions.push_back(sf::Vector2f(size.x - playerVisualSize.x, size.y - playerVisualSize.y));
	// Create the players
	for (size_t i = 0; i < players.size(); ++i) {
		std::shared_ptr<Player> new_player = nullptr;
		sf::Vector2f board_position;
		// The size of the board is hard coded for now
		if (i % 2 != 0) {
			// The board lies on the right
			board_position.x = playerVisualpositions[i].x - 300;
		} else {
			// The board lies on the left
			board_position.x = playerVisualSize.x + 300;
		}
		if (i < 2) {
			// The board lies on the top half of the screen
			board_position.y = 300;
		} else {
			// The board lies on the bottom half of the screen
			board_position.y = playerVisualpositions[i].y + 300;
		}
		switch (players[i]) {
			case HUMAN:
				new_player = std::make_shared<Human>(
					PlayerColour::all_colours()[i], 
					m_bag, 
					board_position
				);
				break;
			case RANDOM:
			default:
				new_player = std::make_shared<RandomAI>(
					PlayerColour::all_colours()[i], 
					m_bag,
					board_position
				);
		}
		g_player_info.addPlayer(new_player);
	}

	for (std::shared_ptr<Player> p : g_player_info.getPlayers()) {
		m_boards.push_back(p->getBoardPtr());
	}

	// Randomise the starting player
	g_player_info.initialiseIndicies();
	g_player_info.setGamePtr(this);

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
	for (int i = 0; i < num_factories; ++i) {
		sf::Vector2f factoryPos = Factory::calculateNewPos(middle, 250, angle);
		m_factories.push_back(std::make_shared<Factory>(i, factoryPos, 60));
		angle += (2 * M_PI / num_factories);
	}
	m_centre = std::make_shared<Factory>(num_factories, middle, 60);
	//m_debug_switchstage.setPosition(playerVisualpositions[2]);
	//m_debug_switchstage.m_callback = []() {
	//	if (g_visual_state == GameState::PICKING) {
	//		g_visual_state = GameState::PLACING;
	//	} else if (g_visual_state == GameState::PLACING) {
	//		g_visual_state = GameState::PICKING;
	//	}
	//};
	// Alternately could use bind?
	//	std::bind(&Game::flipStage, std::ref(*this));

	m_round_visualizer = std::make_unique<RoundVisualizer>(m_bonus_tile_order, sf::Vector2f(size.x/2, size.y *9 / 10));

	fill_factories();
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
		} else if (g_visual_state == GameState::PLACING || g_visual_state == GameState::DISCARDING) {
			std::shared_ptr<Player> currPlayer = g_player_info.getCurrentPlayer();
			currPlayer->getBoardPtr()->onClick(xPos, yPos, *this);
			
			for (auto& vp : m_player_visualizers) {
				vp->onClick(xPos, yPos);
			}
		} else if (g_visual_state == GameState::FINISH) {
			m_finish_screen->onClick(xPos, yPos);
		} else if (g_visual_state == GameState::CHOOSINGREWARD) {
			m_bag->onClick(xPos, yPos);
		}
		//m_debug_switchstage.onClick(xPos, yPos);
		m_thread_running.unlock();
	}
}

void Game::onHover(int xPos, int yPos) {
	if (g_visual_state == GameState::PICKING || g_visual_state == GameState::CHOOSINGREWARD) {
		sf::Drawable* hovered_game_board = nullptr;
		for (auto& pv : m_player_visualizers) {
			if (pv->contains(xPos, yPos)) {
				std::shared_ptr<Player> p = pv->getPlayerVisualized();
				hovered_game_board = p->getBoardPtr();
			}
		}
		g_player_info.setHoveredElement(hovered_game_board);
	}
	if (g_visual_state == GameState::PICKING) {
		for (std::shared_ptr<Factory> factory : m_factories) {
			factory->onHover(xPos, yPos, getBonus());
		}
		m_centre->onHover(xPos, yPos, getBonus());
	} else {
		std::shared_ptr<Player> currPlayer = g_player_info.getCurrentPlayer();
		currPlayer->getBoardPtr()->onHover(xPos, yPos);
	}
	if (g_visual_state != GameState::FINISH) {
		// Always check the player visualizers, since they're always on show
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
	} else if (g_visual_state == GameState::PLACING || g_visual_state == GameState::DISCARDING || g_visual_state == GameState::CHOOSINGREWARD) {
		// Placing stage
		for (Board* b : getVisualisedBoards()) {
			if (b) {
				b->draw(target, states);
			}
		}
		//m_boards[m_current_player.getIndex()]->draw(target, states);
	}

	if (g_visual_state != GameState::FINISH) {
		for (size_t i = 0; i < m_player_visualizers.size(); ++i) {
			m_player_visualizers[i]->draw(target, states);
		}
		if (g_visual_state == GameState::CHOOSINGREWARD) {
			m_bag->draw(target, states);
		}
	} else {
		m_finish_screen->draw(target, states);
	}
	m_round_visualizer->draw(target, states);

	sf::Drawable* temp_elem = g_player_info.getHoveredElement();
	if (temp_elem) {
		target.draw(*temp_elem, states);
	}
	//m_debug_switchstage.draw(target, states);
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
		if (!player->finishedPlacing()) {
			place_tile(choice);
		} else {
        	g_visual_state = GameState::DISCARDING;
		}
	} else if (g_visual_state == GameState::DISCARDING) {
		player->discardDownToFour();
		g_visual_state = GameState::PLACING;
		g_player_info.passOrChangeState();
	} else if (g_visual_state == GameState::CHOOSINGREWARD) {
		player->pickBonusPieces();
		g_player_info.passOrChangeState();
		if (g_visual_state == GameState::CHOOSINGREWARD) {
			g_visual_state = GameState::PLACING;
		}
	}
	updatePlayerVisualizers();
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
	if (m_round_num < m_bonus_tile_order.size() - 1) {
		m_round_num++;
		m_round_visualizer->nextround();
		fill_factories();
		updatePlayerVisualizers();
	} else {
		// End the game
		m_finish_screen = std::make_unique<FinishScreen>(
			m_screen_size,
			g_player_info.getPlayers()
		);
		g_visual_state = GameState::FINISH;
	}
}

void Game::place_tile(PlacingChoice& placed) {
	// The m_current_player var should be correct at this point
	// But lets check anyway
	std::shared_ptr<Player> player = g_player_info.getCurrentPlayer();
	if (player->finishedPlacing() || placed.star == nullptr) {
		g_logger.log(Logger::ERROR, "Place_tile called when current player has finished placing");
		if (!g_player_info.playerNotFinished()) {
			switchToPickingStage();
		}
		g_player_info.nextTurn();
		updatePlayerVisualizers();
		return;
	}

	player->resolvePlacingChoice(placed, getBonus());
	// This might have changed the game state, so we should check that hasn't happen
	if (g_visual_state != GameState::CHOOSINGREWARD) {
		g_player_info.passOrChangeState();
	}
	updatePlayerVisualizers();
}

void Game::placing_stage() {
	std::cout << "\n========================\nPLACING STAGE\n";
	for (std::shared_ptr<Player> player : g_player_info.getPlayers()) {
		player->resetDonePlacing();
	}
	while (g_player_info.playerNotFinished()) {
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
