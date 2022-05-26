#include <players/PlayerInfo.hpp>

#include <Game.hpp>

void PlayerInfo::passOrChangeState() {
    if (playerNotFinished()) {
        // This could be better? but I can't think of how right now
        bool lookingForNextPlayer = true;
        while (lookingForNextPlayer) {
            nextTurn();
            if (!getCurrentPlayer()->finishedPlacing()) {
                lookingForNextPlayer = false;
            }
        }
    } else {
        m_game_ptr->switchToPickingStage();
    }
    updateVisuals();
}

void PlayerInfo::updateVisuals() {
    if (m_game_ptr)
        m_game_ptr->updatePlayerVisualizers();
}