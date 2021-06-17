#ifndef TILE
#define TILE

#include <vector>
#include <string>
#include <cmath>
#include <SFML/Graphics.hpp>

using namespace sf;

static float DEFAULT_TILE_SIZE_X = 50;
static float DEFAULT_TILE_SIZE_Y = 30;

class Tile : public ConvexShape {
public:
	enum Type {
		ORANGE,
		RED,
		BLUE,
		YELLOW,
		GREEN,
		PURPLE,
		NONE
	};
	static std::vector<Tile> all_tiles() {
		return {
			Tile(ORANGE),
			Tile(RED),
			Tile(BLUE),
			Tile(YELLOW),
			Tile(GREEN),
			Tile(PURPLE)
		};
	}
	Tile(Type t) : m_t(t), m_size(Vector2f(DEFAULT_TILE_SIZE_X,DEFAULT_TILE_SIZE_Y)) {
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
	const bool operator==(Tile t) {
		return m_t == t.m_t;
	}
	const bool operator==(Type t) {
		return m_t == t;
	}
	const bool operator!=(Tile t) {
		return m_t != t.m_t;
	}
	const bool operator!=(Type t) {
		return m_t != t;
	}
	const Type colour() {
		return m_t;
	}
private:
	void setupColour() {
		switch (m_t) {
			default:
			case ORANGE: setFillColor(Color(235,149,52));
			break;
			case RED: setFillColor(Color(235,52,52));
			break;
			case BLUE: setFillColor(Color(52,137,235));
			break;
			case YELLOW: setFillColor(Color(235,229,52));
			break;
			case GREEN: setFillColor(Color(70,235,52));
			break;
			case PURPLE: setFillColor(Color(171,52,235));
			break;
			case NONE: setFillColor(Color(0,0,0,0));
		}
	}
	Type m_t;
	Vector2f m_size = Vector2f(0,0);
	std::vector<std::string> strings = {
		"Orange",
		"Red",
		"Blue",
		"Yellow",
		"Green",
		"Purple",
		"None"
	};
};

#endif
