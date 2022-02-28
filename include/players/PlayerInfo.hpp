#ifndef PLAYERINFO
#define PLAYERINFO

#include <players/Player.hpp>
#include <game_elements/PlayerVisualizer.hpp>

class PlayerInfo {
public:
    std::shared_ptr<Player> getCurrentPlayer() {
	    return players[current_player.getIndex()];
    }
    void nextTurn() {
        current_player++;
    }
    // Sets the starting player to the current player
    void setStartingPlayer() {
        starting_player = current_player.getIndex();
    }
    void addPlayer(std::shared_ptr<Player> p) {
        players.push_back(p);
    }
    std::vector<std::shared_ptr<Player>> getPlayers() {
        return players;
    }
    std::shared_ptr<Player> getPlayer(int index) {
        if (index >= players.size()) {
            g_logger.log(Logger::ERROR, "getPlayer Index out of range");
        }
        return players[index];
    }
    size_t numPlayers() {
        return players.size();
    }
    // This should be called after all players are added via addPlayer
    void initialiseIndicies() {
        starting_player = rand() % players.size();
        current_player = cIndex(starting_player + 1, players.size());
    }
private:
    cIndex current_player;
	int starting_player;
	std::vector<std::shared_ptr<Player>> players;
};

#endif