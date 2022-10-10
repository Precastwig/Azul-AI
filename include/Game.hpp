#ifndef GAME
#define GAME

// SFML includes
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>

// std includes
#include <utility>
#include <vector>
#include <memory>
#include <mutex>

// Internal includes
#include <players/Player.hpp>
#include "utils/helper_enums.hpp"
#include <utils/Choices.hpp>
#include <game_elements/RoundVisualizer.hpp>
#include <game_elements/Bag.hpp>
#include <game_elements/Factory.hpp>
#include <game_elements/PlayerVisualizer.hpp>
#include <ui_elements/FinishScreen.hpp>
#include <ui_elements/Button.hpp>

class Game : public sf::Drawable {
public:
	// Constructor/Destructor
	Game(std::vector<std::pair<PlayerType, PlayerColour::Colour>> players, sf::Vector2f size = sf::Vector2f());
	~Game();

	virtual void draw (sf::RenderTarget &target, sf::RenderStates states) const override;

	void onClick(int xPos, int yPos);
	void onHover(int xPos, int yPos);
	void onLeft();
	void onRight();
	
	//-------------------------------------------------------------------
	// SFML specific functions
	void performAIActions();
	void performAIAction(std::shared_ptr<Player> player);
	void switchToPlacingStage();
	void switchToPickingStage();

	//-------------------------------------------------------------------
	// Command line specific functions
	// Initiates the play sequence
	void play();
	// Prints the factories to stdout
	void printFactories();
	// Print some info to stdout declaring the winner of the game
	void declare_winner();
	// Players take turns placing tiles on their personal boards until they choose to stop (or can't place any more tiles)
	void placing_stage();
	// Players take turns picking tiles from factories until all are empty
	void picking_stage();

	// Turn step functions

	// Pulls tiles from the bag and places on the factories until each
	// factory contains 4, or the bag runs out
	void fill_factories();

	void pick_tile(PickingChoice& picked);
	void place_tile(PlacingChoice& placed);

	//-------------------------------------------------------------------
	// Helper functions

	// Check if all factories are empty
	bool noTilesLeft();
	// Get the current bonus tile type
	TileType getBonus();

	void updatePlayerVisualizers();
private:
	std::vector<Board*> getVisualisedBoards() const;
	void positionTiles();

	//-------------------------------------------------------------------
	// Member variables
	// Six rounds
	std::vector<TileType> m_bonus_tile_order = {
		TileType::PURPLE,
		TileType::GREEN,
		TileType::ORANGE,
		TileType::YELLOW,
		TileType::BLUE,
		TileType::RED
	};

	// Tile sources
	std::shared_ptr<Bag> m_bag;
	Button m_bag_show_button;

	// Central tiles
	std::vector<std::shared_ptr<Factory>> m_factories;
	std::shared_ptr<Factory> m_centre;

	// Boards, these are non-owning pointers
	std::vector<Board*> m_boards;

	// Current round info
	int m_round_num;
	Button m_debug_switchstage;
	// Used only in SFML implementation
	std::unique_ptr<FirstTile> m_first_tile;
	std::mutex m_thread_running;
	sf::Vector2f m_screen_size;

	//std::vector<std::shared_ptr<sf::Texture>> m_textures;
	std::unique_ptr<FinishScreen> m_finish_screen;
	std::unique_ptr<RoundVisualizer> m_round_visualizer;
	std::vector<std::unique_ptr<PlayerVisualizer>> m_player_visualizers;
};

#endif
