#include "DelayedSound.h"

namespace tt
{

DelayedSoundPtr DelayedSound::create(const std::string& name, float delay, ResourceManager& resources)
{
    auto buffer = resources.cacheSound(name);
    
    DelayedSoundPtr retval;
    if (buffer)
    {
        retval = std::make_unique<DelayedSound>(*buffer);
    }
    else
    {
        retval = std::make_unique<DelayedSound>();
    }

    retval->setDelay(delay);

    return std::move(retval);
}

DelayedSound::DelayedSound(const sf::SoundBuffer& buffer)
{
    _thesound.setBuffer(buffer);
}

void DelayedSound::play()
{
    if (_clock.getElapsedTime().asMilliseconds() > _delay)
    {
        _thesound.play();
        _clock.restart();
    }
}

} // namespace tt