#ifndef TILE
#define TILE

#include "Logger.hpp"
#include "Colours.hpp"
#include "ui_elements/CustomShape.hpp"
#include "utils/Sounds.hpp"
#include <SFML/Graphics/Color.hpp>
#include <vector>
#include <string>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <iostream>

extern Logger g_logger;

static float DEFAULT_TILE_SIZE_X = 50;
static float DEFAULT_TILE_SIZE_Y = 30;

static std::vector<std::string> strings = {
	"Orange",
	"Red",
	"Blue",
	"Yellow",
	"Green",
	"Purple",
	"None"
};

class Tile : public CustomShape {
public:
	static std::vector<TileType> all_tile_types() {
		return {
			ORANGE,
			RED,
			BLUE,
			YELLOW,
			GREEN,
			PURPLE
		};
	}
	Tile(TileType t, sf::Vector2f size = sf::Vector2f(DEFAULT_TILE_SIZE_X,DEFAULT_TILE_SIZE_Y)) : m_t(t), m_size(size), m_hover_thickness(2.0) {
		setPointCount(4);
		setPoint(0, sf::Vector2f(0,m_size.y));
		setPoint(1, sf::Vector2f(-m_size.x,0));
		setPoint(2, sf::Vector2f(0, -m_size.y));
		setPoint(3, sf::Vector2f(m_size.x, 0));
		setRotation(rand() % 180);
		setupColour();
		update();
	};

	const std::string toString() {
		return strings[m_t];
	}
	static std::string toString(TileType t) {
		return strings[t];
	}
	const bool operator==(Tile t) {
		return m_t == t.m_t;
	}
	const bool operator==(TileType t) {
		return m_t == t;
	}
	const bool operator!=(Tile t) {
		return m_t != t.m_t;
	}
	const bool operator!=(TileType t) {
		return m_t != t;
	}
	const TileType colour() {
		return m_t;
	}
	void setType(TileType t) {
		m_t = t;
	}
	void setOpacity(int newOpacity) {
		sf::Color fc = getFillColor();
		fc.a = newOpacity;
		setFillColor(fc);
		sf::Color oc = getOutlineColor();
		oc.a = newOpacity;
		setOutlineColor(oc);
	}
	sf::Vector2f getSize() {
		return m_size;
	}
private:
	void setupColour() {
		setFillColor(Colours::tile_to_col(m_t));
	}
	TileType m_t;
	sf::Vector2f m_size = sf::Vector2f(0,0);
	float m_hover_thickness;
};

#endif
