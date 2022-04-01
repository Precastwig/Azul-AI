#include "utils/Sounds.hpp"
#include "Logger.hpp"

extern Logger g_logger;

std::vector<sf::Sound> m_sounds;
sf::SoundBuffer m_pop_sound_buff;
sf::SoundBuffer m_click_sound_buff;
sf::SoundBuffer m_dong_sound_buff;
sf::SoundBuffer m_crumple_sound_buff;

Sounds::Sounds() {
    if (!m_pop_sound_buff.loadFromFile("resources/sfx-pop.flac"))
        g_logger.log(Logger::ERROR, "sfx-pop.flac not loaded");
    if (!m_click_sound_buff.loadFromFile("resources/click.flac"))
        g_logger.log(Logger::ERROR, "click.flac not loaded");
    if (!m_dong_sound_buff.loadFromFile("resources/dong.flac"))
        g_logger.log(Logger::ERROR, "dong.flac not loaded");
    if (!m_crumple_sound_buff.loadFromFile("resources/crumple.flac"))
        g_logger.log(Logger::ERROR, "crumple.flac not loaded");

    m_sounds.push_back(sf::Sound());
}

sf::Sound& Sounds::get_unused_sound_obj(){
    for (size_t i; i < m_sounds.size(); ++i) {
        if (m_sounds[i].getStatus() != sf::Sound::Playing) {
            return m_sounds[i];
        }
    }
    m_sounds.push_back(sf::Sound());
    return m_sounds[m_sounds.size() - 1];
}

void Sounds::pop(float pitch) {
    sf::Sound& sound = get_unused_sound_obj();
    sound.setBuffer(m_pop_sound_buff);
    sound.setPitch(pitch);
    sound.play();
}

void Sounds::click(float pitch) {
    sf::Sound& sound = get_unused_sound_obj();
    sound.setBuffer(m_click_sound_buff);
    sound.setPitch(pitch);
    sound.play();
}

void Sounds::dong(float pitch) {
    sf::Sound& sound = get_unused_sound_obj();
    sound.setBuffer(m_dong_sound_buff);
    sound.setPitch(pitch);
    sound.play();
}

void Sounds::crumple(float pitch) {
    sf::Sound& sound = get_unused_sound_obj();
    sound.setBuffer(m_crumple_sound_buff);
    sound.setPitch(pitch);
    sound.play();
}