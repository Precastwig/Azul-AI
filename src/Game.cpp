#include "Game.hpp"
#include "players/RandomAI.hpp"
#include "players/HumanCommandLine.hpp"
#include <algorithm>
#include <iostream>

Game::Game() : m_centre_taken(false), m_bonus_type(m_bonus_tile_order[0]) {
	// Create the bag
	m_bag = std::make_shared<Bag>();
	// Create the players, for now one randomAI and one humancmd

	std::shared_ptr<Player> randomai = std::make_shared<RandomAI>(PlayerColour::all_colours()[0], m_bag);
	std::shared_ptr<Player> player = std::make_shared<HumanCommandLine>(PlayerColour::all_colours()[1], m_bag);
	m_players.push_back(randomai);
	m_players.push_back(player);

	// Randomise the starting player
	m_starting_player = rand() % m_players.size();
	m_current_player = cIndex(m_starting_player, m_players.size());

	// Create the factories
	int num_factories = (m_players.size() * 2) + 1;
	double angle = 0.0;
	sf::Vector2f middle(500,500);
	for (int i = 0; i < num_factories; ++i) {
		sf::Vector2f factoryPos = Factory::calculateNewPos(middle, 300, angle);
		m_factories.push_back(std::make_shared<Factory>(i, factoryPos, 60));
		angle += (2 * M_PI / num_factories);
	}
	m_centre = std::make_shared<Factory>(num_factories, middle, 60);
}

Game::~Game() {
}

void Game::onClick(int xPos, int yPos) {
	// Find the object that we've clicked on and call its onClick event
	for (std::shared_ptr<Factory> factory : m_factories) {
		if (factory->contains(xPos, yPos)) {
			factory->onClick(xPos, yPos, *this);
		}
	}
	if (m_centre->contains(xPos, yPos)) {
		m_centre->onClick(xPos, yPos, *this);
	}
}

void Game::draw (RenderTarget &target, RenderStates states) const {
	m_centre->draw(target, states);
	for (std::shared_ptr<Factory> factory : m_factories) {
		factory->draw(target, states);
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
		picking_stage();
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
		if (!centreTaken && choice.factory == m_centre) {
			// Someone has taken the centre!
			centreTaken = true;
			player->minusPoisonPoints();
			m_starting_player = player_turn.getIndex();
		}
		player->resolvePickingChoice(choice, m_bonus_type, m_centre);
		player_turn++;
		printFactories();
	}
}

void Game::pick_tile(PickingChoice& picked) {
	std::shared_ptr<Player> player = m_players[m_current_player.getIndex()];
	if (!m_centre_taken && picked.factory == m_centre) {
		// Someone has taken from the centre
		m_centre_taken = true;
		player->minusPoisonPoints();
		m_starting_player = m_current_player.getIndex();
	}
	std::string withBonus = (picked.with_bonus) ? "with a bonus tile" : "";
	g_logger.log(Logger::INFO, player->toShortString() + " has picked " + std::to_string(picked.factory->numberOf(picked.tile_colour)) + " " + Tile::toString(picked.tile_colour) + " " + withBonus);

	player->resolvePickingChoice(picked, m_bonus_type, m_centre);
	m_current_player++;
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
