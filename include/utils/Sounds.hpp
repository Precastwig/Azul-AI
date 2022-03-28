#ifndef SOUNDS
#define SOUNDS

#include <SFML/Audio.hpp>

class Sounds {
public:
    Sounds();

    void pop(float pitch = 1.0);

private:
    sf::Sound m_sound;
    sf::SoundBuffer m_pop_sound_buff;
};

#endif