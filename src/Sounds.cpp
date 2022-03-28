#include "utils/Sounds.hpp"
#include "Logger.hpp"

extern Logger g_logger;

Sounds::Sounds() {
    if (!m_pop_sound_buff.loadFromFile("resources/sfx-pop.flac"))
        g_logger.log(Logger::ERROR, "sfx-pop2.wav not loaded");
}

void Sounds::pop(float pitch) {
    m_sound.setBuffer(m_pop_sound_buff);
    m_sound.setPitch(pitch);
    m_sound.play();
}