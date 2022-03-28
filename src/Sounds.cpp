#include "utils/Sounds.hpp"
#include "Logger.hpp"

extern Logger g_logger;

sf::Sound m_sound;
sf::Sound m_dong_sound;
sf::SoundBuffer m_pop_sound_buff;
sf::SoundBuffer m_click_sound_buff;
sf::SoundBuffer m_dong_sound_buff;

Sounds::Sounds() {
    if (!m_pop_sound_buff.loadFromFile("resources/sfx-pop.flac"))
        g_logger.log(Logger::ERROR, "sfx-pop.flac not loaded");
    if (!m_click_sound_buff.loadFromFile("resources/click.flac"))
        g_logger.log(Logger::ERROR, "click.flac not loaded");
    if (!m_dong_sound_buff.loadFromFile("resources/dong.flac"))
        g_logger.log(Logger::ERROR, "dong.flac not loaded");
}

void Sounds::pop(float pitch) {
    m_sound.setBuffer(m_pop_sound_buff);
    m_sound.setPitch(pitch);
    m_sound.play();
}

void Sounds::click(float pitch) {
    m_sound.setBuffer(m_click_sound_buff);
    m_sound.setPitch(pitch);
    m_sound.play();
}

void Sounds::dong(float pitch) {
    m_dong_sound.setBuffer(m_dong_sound_buff);
    m_dong_sound.setPitch(pitch);
    m_dong_sound.play();
}