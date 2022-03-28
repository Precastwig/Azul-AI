#ifndef SOUNDS
#define SOUNDS

#include <SFML/Audio.hpp>

class Sounds {
public:
    Sounds();

    static void pop(float pitch = 1.0);
    static void click(float pitch = 1.0);
    static void dong(float pitch = 1.0);
};

#endif