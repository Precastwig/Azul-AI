#ifndef TILE
#define TILE

#include <vector>
#include <string>
#include <cmath>
#include <SFML/Graphics.hpp>

using namespace sf;

static float DEFAULT_TILE_SIZE = 75;
static float DEFAULT_ANGLE = 0.785398; // 45 degrees

class Tile : public Shape {
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
	Tile(Type t) : m_t(t), m_size(Vector2f(DEFAULT_TILE_SIZE,DEFAULT_TILE_SIZE)), m_angle(DEFAULT_ANGLE) {
		setupColour();
		update();
	};
	// Tile(Type t, Vector2f size, float angle) : m_t(t), m_size(size), m_angle(angle) {
	// 	setupColour();
	// 	update();
	// };

	virtual std::size_t getPointCount() const override {
		return 4;
	}

	virtual Vector2f getPoint(std::size_t index) const override {
		switch (index) {
			default:
			case 0: return Vector2f(0,0);
			case 1: return Vector2f(m_size.x,0);
			case 2: return Vector2f((m_size.y * cos(m_angle)) + m_size.x,(m_size.y * sin(m_angle)));
			case 3: return m_size.y * Vector2f(cos(m_angle),sin(m_angle));
		}
	}

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
	float m_angle = 0;
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
