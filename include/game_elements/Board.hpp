#ifndef BOARD
#define BOARD

#include <SFML/Graphics/Drawable.hpp>
#include <string>
#include <vector>
#include <memory>
#include <utils/helper_enums.hpp>
#include <utils/cIndex.hpp>
#include <utils/Choices.hpp>
#include <game_elements/Location.hpp>

// Pre declarations
class Player;

class Board : sf::Drawable {
public:
	Board();

	void onHover(int xpos, int ypos);

	virtual void draw (sf::RenderTarget &target, sf::RenderStates states) const override;

	static std::vector<TileType> getAdjacentStarColours(TileType starCol);

	void placeTile(PlacingChoice choice, Player* me);
	void keepTiles(std::vector<std::shared_ptr<Tile>> to_keep);

	// This is always talking about the column clockwise around the given colour
	int tilesNeededToGetStatue(Location::Types loc);
	int tilesNeededToGetWindow(Location::Types loc);

	std::vector<PlacingChoice> getPlacingChoicesOfCol(TileType col);
	std::vector<PlacingChoice> getAllPlacingChoices();
	std::vector<TileType> getUnusedColoursInCentre();

	std::string toString(std::shared_ptr<Location> star);
	std::string toString();
private:
	int bonusPiecesAwarded();
	int bonusPointsAwarded();

	// The bonus tiles
	// One window per colour (5 and 6)
	std::vector<bool> m_windows;
	// One statue per colour (where the 1 and 2 of that colour is)
	std::vector<bool> m_statues;
	// One column per colour, (where the 2 and 3 of that colour is)
	std::vector<bool> m_columns;

	// The bonus points
	std::vector<bool> m_full_numbers;

	// The tile spaces
	std::vector<std::shared_ptr<Location>> m_stars;
	std::vector<std::shared_ptr<Tile>> m_keep;
	std::vector<TileType> m_colours_not_in_centre;
};

#endif
