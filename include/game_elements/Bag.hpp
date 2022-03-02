#ifndef BAG
#define BAG

#include "utils/helper_enums.hpp"
#include "game_elements/Factory.hpp"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <memory>

// This class ecompasses the bag of tiles, the bin, and the reward tile selection
// aka, all locations that tiles originate, and go to die
class Bag : public sf::Drawable {
public:
	Bag(sf::Vector2f size, sf::Vector2f position);

	virtual void draw (RenderTarget &target, RenderStates states) const override;

	void fillFactory(std::shared_ptr<Factory> f);

	void toBin(std::vector<std::shared_ptr<Tile>> tiles);

	// void toBin(Tile colour, int number);

	// This function removes the given list of tiles from the
	// reward tiles display
	void takeRewardTiles(std::vector<std::shared_ptr<Tile>> tiles);

	// Gets current selection of reward tiles
	std::vector<std::shared_ptr<Tile>> rewardTiles() const {
		return m_reward_tiles;
	};

private:
	// Functions
	void binToBag();
	std::shared_ptr<Tile> pullTile();

	// Visualization Variables
	sf::Text m_txt;
	sf::RectangleShape m_background;

	// Storage Variables
	std::vector<std::shared_ptr<Tile>> m_tile_bin;
	std::vector<std::shared_ptr<Tile>> m_tile_bag;
	// Reward tiles
	std::vector<std::shared_ptr<Tile>> m_reward_tiles;
};

#endif
