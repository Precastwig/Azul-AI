#include "player.hpp"

Player::Player(PlayerColour colour, Game* game)
	: m_game(game), m_col(colour), m_board(), m_points(0) {
};

void Player::addPoints(int points) {
	m_points += points;
}
