#ifndef FACTORY
#define FACTORY

#include "utils/helper_enums.hpp"
#include <stdlib.h>
#include <memory>
#include <SFML/Graphics.hpp>

using namespace sf;

class Factory : public CircleShape {
public:
	Factory(const int id, const sf::Vector2f position, const float size) : m_id(id), m_position(position), m_size(size), m_tiles(), m_background() {
		m_background.setRadius(size * 2);
		m_background.setPosition(position - sf::Vector2f(size*2, size*2 ));
		m_background.setFillColor(Color(255,203,208));
	};
	~Factory() = default;

	virtual void draw (RenderTarget &target, RenderStates states) const override;

	static sf::Vector2f calculateNewPos(const sf::Vector2f& oldPos, const float& size, const double& angle);

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
	const sf::Vector2f m_position;
	const float m_size;
	// Drawable elements
	std::vector<Tile> m_tiles;
	sf::CircleShape m_background;
};

#endif
