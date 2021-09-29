#ifndef FACTORY
#define FACTORY

#include "utils/helper_enums.hpp"
#include "Logger.hpp"
#include <stdlib.h>
#include <memory>
#include <SFML/Graphics.hpp>

using namespace sf;

class Game;
extern Logger g_logger;

class Factory : public Drawable {
public:
	Factory(const int id, const sf::Vector2f position, const float size) : m_id(id), m_size(size), m_tiles(), m_background() {
		m_background.setOrigin(size*2,size*2);
		m_background.setRadius(size * 2);
		m_background.setPosition(position);
		m_background.setFillColor(Color(255,203,208));
	};
	~Factory() = default;

	virtual void draw (RenderTarget &target, RenderStates states) const override;

	static sf::Vector2f calculateNewPos(const sf::Vector2f& oldPos, const float& size, const double& angle);

	bool contains(int x, int y);
	void onClick(int x, int y, Game& game);

	// Add tiles to the factory
	void place(Tile tile);
	void addTiles(std::vector<Tile> tiles);
	// Remove tiles from the factory
	void removeTiles(Tile tile_taken, Tile bonus_type, std::shared_ptr<Factory> centre);

	// Getter functions
	std::vector<Tile> tiles() const;
	bool isEmpty() const;
	int numberOf(Tile tile) const;
	bool hasBonus(Tile bonus) const;
	bool isOnlyBonus(Tile bonus) const;

	std::string toString();
	int id() {return m_id;};
private:
	const int m_id;
	const float m_size;
	// Drawable elements
	std::vector<Tile> m_tiles;
	sf::CircleShape m_background;
};

#endif
