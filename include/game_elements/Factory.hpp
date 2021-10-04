#ifndef FACTORY
#define FACTORY

#include "utils/helper_enums.hpp"
#include "Logger.hpp"
#include <stdlib.h>
#include <memory>
#include <SFML/Graphics.hpp>

using namespace sf;

class Game;
class Factory;
extern Logger g_logger;

class Factory : public std::enable_shared_from_this<Factory>, Drawable {
public:
	Factory(const int id, const sf::Vector2f position, const float size);
	~Factory() = default;

	virtual void draw (RenderTarget &target, RenderStates states) const override;

	static sf::Vector2f calculateNewPos(const sf::Vector2f& oldPos, const float& size, const double& angle);

	bool contains(int x, int y);
	void onClick(int x, int y, Game& game);

	// Add tiles to the factory
	void place(std::shared_ptr<Tile> tile);
	void addTiles(std::vector<std::shared_ptr<Tile>> tiles);
	// Remove tiles from the factory, returns the removed tiles
	std::vector<std::shared_ptr<Tile>> removeTiles(Tile::Type colour_taken, Tile::Type bonus_type, std::shared_ptr<Factory> centre);

	// Getter functions
	std::vector<std::shared_ptr<Tile>> tiles() const;
	bool isEmpty() const;
	int numberOf(Tile::Type tile) const;
	bool hasBonus(Tile::Type bonus) const;
	bool isOnlyBonus(Tile::Type bonus) const;
	void positionTiles() const;

	std::string toString();
	int id() {return m_id;};
private:
	const int m_id;
	const float m_size;
	// Drawable elements
	std::vector<std::shared_ptr<Tile>> m_tiles;
	sf::CircleShape m_background;
};

#endif
