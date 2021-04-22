#ifndef FACTORY
#define FACTORY

#include "utils/helper_enums.hpp"
#include <stdlib.h>
#include <memory>
#include <SFML/Graphics.hpp>

using namespace sf;

class Factory : public Drawable {
public:
	Factory(const int id) : m_id(id), m_tiles() {};
	~Factory() = default;

	virtual void draw (RenderTarget &target, RenderStates states) const override;

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
	std::vector<Tile> m_tiles;
};

#endif
