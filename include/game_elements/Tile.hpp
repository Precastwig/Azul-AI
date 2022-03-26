#ifndef TILE
#define TILE

#include "Logger.hpp"
#include "Colours.hpp"
#include "ui_elements/CustomShape.hpp"
#include <vector>
#include <string>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <iostream>

extern Logger g_logger;

using namespace sf;

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
	Tile(TileType t, Vector2f size = Vector2f(DEFAULT_TILE_SIZE_X,DEFAULT_TILE_SIZE_Y)) : m_t(t), m_size(size) {
		setPointCount(4);
		setPoint(0, Vector2f(0,DEFAULT_TILE_SIZE_Y));
		setPoint(1, Vector2f(-DEFAULT_TILE_SIZE_X,0));
		setPoint(2, Vector2f(0, -DEFAULT_TILE_SIZE_Y));
		setPoint(3, Vector2f(DEFAULT_TILE_SIZE_X, 0));
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
	Vector2f getSize() {
		return m_size;
	}
private:
	void setupColour() {
		setFillColor(Colours::tile_to_col(m_t));
	}
	TileType m_t;
	Vector2f m_size = Vector2f(0,0);
};

#endif
