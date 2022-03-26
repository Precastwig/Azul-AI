#ifndef HELPER_ENUMS
#define HELPER_ENUMS

#include <SFML/Graphics/Drawable.hpp>
#include <memory>
#include <vector>
#include <string>
#include <utils/cIndex.hpp>

enum MenuState {
	MAIN,
	NEWGAME,
	SETTINGS,
	OFF
};

class GameState {
public:
	// Basic checkers
	bool is_picking() {return m_state == PICKING;}
	void set_picking() {m_state = PICKING;}
	bool is_finish() {return m_state == FINISH;}
	void set_finish() {m_state = FINISH;}
	bool is_placing() {return m_state == PLACING;}
	void set_placing() {m_state = PLACING;}
	bool is_discarding() {return m_state == DISCARDING;}
	void set_discarding() {m_state = DISCARDING;}
	bool is_reward() {return m_state == CHOOSINGREWARD;}
	void set_reward() {m_state = CHOOSINGREWARD;}

	// Complex checkers
	bool is_popup_game_board() {
		return m_state == PICKING || m_state == CHOOSINGREWARD;
	}
	bool is_player_visualizers_clickable() {
		return m_state == DISCARDING || m_state == PLACING;
	}
	bool is_player_visualizers_visable() {
		return m_state != FINISH && m_state != NOSTATE;
	}
	bool is_boards_visualised() {
		return m_state == GameState::PLACING || 
			m_state == GameState::DISCARDING || 
			m_state == GameState::CHOOSINGREWARD;
	}
	bool is_discard_button_clickable() {
		return m_state == PLACING || m_state == DISCARDING;
	}

	enum {
		PICKING,
		PLACING,
		DISCARDING,
		CHOOSINGREWARD,
		FINISH,
		NOSTATE // For pause menu or something?
	} m_state;
};


enum PlayerType {
	HUMAN,
	RANDOM,
	COLOURTARGET
};

enum TileType {
	ORANGE,
	RED,
	BLUE,
	YELLOW,
	GREEN,
	PURPLE,
	NONE
};

enum LocationType {
	ORANGE_STAR,
	RED_STAR,
	BLUE_STAR,
	YELLOW_STAR,
	GREEN_STAR,
	PURPLE_STAR,
	CENTRE_STAR
};

// This is not a good class composition, should be all static
class PlayerColour {
public:
	enum Colour {
		WHITE,
		WOOD,
		BLACK,
		GREY
	};
	static std::vector<Colour> all_colours() {
		return {
		WHITE,
		WOOD,
		BLACK,
		GREY};
	};
	// So dumb
	static std::string toString(Colour c) {
		PlayerColour temp(c);
		return temp.toString();
	}
	const std::string toString() {
		return strings[m_c];
	}
	PlayerColour(Colour c) : m_c(c) {};
	const bool operator==(PlayerColour p) {
		return m_c == p.m_c;
	}
	const bool operator!=(PlayerColour p) {
		return m_c != p.m_c;
	}
private:
	Colour m_c;
	std::vector<std::string> strings = {
		"White",
		"Wood",
		"Black",
		"Grey"
	};
};

struct Cost {
	TileType colour = TileType::NONE;
	int num_colour = 0;
	int num_bonus = 0;
};

static const std::vector<int> all_nums_points = {
	4, 8, 12, 16
};

#endif
