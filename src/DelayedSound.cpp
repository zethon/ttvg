#include "AudioService.h"
#include "DelayedSound.h"

namespace tt
{

DelayedSoundPtr DelayedSound::create(const std::string& name, float delay, ResourceManager& resources)
{
    auto retval = std::make_unique<DelayedSound>(name);
    retval->setDelay(delay);
    return retval;
}

DelayedSound::DelayedSound(const std::string& name)
    : _name{ name }
{
    AudioLocator::sound()->cacheAudio(name);
}

void DelayedSound::play()
{
    if (_clock.getElapsedTime().asMilliseconds() > _delay)
    {
        AudioLocator::sound()->play(_name);
        _clock.restart();
    }
}

} // namespace tt