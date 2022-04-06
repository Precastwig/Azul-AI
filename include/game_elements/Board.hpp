#ifndef BOARD
#define BOARD

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>
#include <memory>
#include <utils/helper_enums.hpp>
#include <utils/cIndex.hpp>
#include <utils/Choices.hpp>

// Pre declarations
class Player;

class Board : public sf::Drawable {
public:
	Board(sf::Vector2f position, sf::Vector2f total_size, bool display_bonus_right, int bonus_size);

	// Input responding functions
	void onHover(int xpos, int ypos, Game& game);
	void onClick(int x, int y, Game& game);
	void onLeft(TileType bonus);
	void onRight(TileType bonus);

	// Sets the various visual elements for when the game is in a nonplacing state
	void setTempVisualState(); 
	// The opposite of the above function 
	void setRegularVisualState();

	virtual void draw (sf::RenderTarget &target, sf::RenderStates states) const override;

	static std::vector<TileType> getAdjacentStarColours(TileType starCol);

	void placeTile(PlacingChoice choice, Player* me);
	void keepTiles(std::vector<std::shared_ptr<Tile>> to_keep);

	// This is always talking about the column clockwise around the given colour
	int tilesNeededToGetStatue(LocationType loc);
	int tilesNeededToGetWindow(LocationType loc);

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

	// Bonus markers
	bool m_display_bonus_on_right;
	std::vector<std::shared_ptr<Tile>> m_bonus_point_nums;
	std::vector<sf::Text> m_bonus_point_nums_nums;
	std::vector<std::shared_ptr<Tile>> m_bonus_point_colours;
	sf::RectangleShape m_bonus_background;

	// The tile spaces
	std::vector<std::shared_ptr<Location>> m_stars;
	std::vector<std::shared_ptr<Tile>> m_keep;
	std::vector<TileType> m_colours_not_in_centre;
	sf::RectangleShape m_background;
};

#endif
