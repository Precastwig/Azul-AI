#ifndef TILE
#define TILE

#include "Logger.hpp"
#include "Colours.hpp"
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

class Tile : public ConvexShape {
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
	Tile(TileType t) : m_t(t), m_size(Vector2f(DEFAULT_TILE_SIZE_X,DEFAULT_TILE_SIZE_Y)) {
		setPointCount(4);
		setPoint(0, Vector2f(0,DEFAULT_TILE_SIZE_Y));
		setPoint(1, Vector2f(-DEFAULT_TILE_SIZE_X,0));
		setPoint(2, Vector2f(0, -DEFAULT_TILE_SIZE_Y));
		setPoint(3, Vector2f(DEFAULT_TILE_SIZE_X, 0));
		setRotation(rand() % 180);
		setupColour();
		update();
	};

	const bool contains(int x, int y) {
		Vector2f tp(x, y);
		std::vector<Vector2f> vertices = getVertices();
	    int pos = 0;
	    int neg = 0;
		// g_logger.log(Logger::INFO, "Contains");
		// g_logger.log(Logger::INFO, "Position:");
		// g_logger.log(Logger::INFO, getPosition());

	    for (unsigned int i = 0; i < vertices.size(); i++){
			if (vertices[i] == tp) {
				return true;
			}
			// Form a segment between two points
			Vector2f point1 = vertices[i];
			Vector2f point2 = vertices[(i+1)%vertices.size()];
			// Compute the cross product
			int cross = (tp.x - point1.x)*(point2.y - point1.y) - (tp.y - point1.y)*(point2.x - point1.x);
			if (cross > 0) pos++;
	        if (cross < 0) neg++;
			// If the sign changes, then no
			if (pos > 0 && neg > 0) {
				return false;
			}
	    }
		return true;
	}
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
private:
	const std::vector<Vector2f> getVertices() {
		Transform t = getTransform();
		return {t.transformPoint(getPoint(0)),
			t.transformPoint(getPoint(1)),
			t.transformPoint(getPoint(2)),
			t.transformPoint(getPoint(3))};
	}
	void setupColour() {
		setFillColor(Colours::tile_to_col(m_t));
	}
	TileType m_t;
	Vector2f m_size = Vector2f(0,0);
	sf::ConvexShape m_shape;
};

#endif
