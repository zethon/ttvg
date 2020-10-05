#pragma once 
#include <memory>

#include <SFML/Audio.hpp>

#include "ResourceManager.h"

namespace tt
{

class DelayedSound;
using DelayedSoundPtr = std::unique_ptr<DelayedSound>;

class DelayedSound
{

public:
    static DelayedSoundPtr create(const std::string& name,
        float delay, 
        ResourceManager& resources);

    DelayedSound(const sf::SoundBuffer& buffer);
    DelayedSound() = default;

    float delay() const { return _delay; }
    void setDelay(float v) { _delay = v; }

    void setVolume(float v) { _thesound.setVolume(v); }

    void play();

private:
    sf::Sound   _thesound;
    sf::Clock   _clock;
    float       _delay = 0.f;

};

} // namesapce tt