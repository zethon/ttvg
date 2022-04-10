#include "ResourceManager.h"
#include "AudioService.h"

namespace tt
{

void SfxAudio::cacheAudio(const std::string& name)
{
    auto buffer = _resources.cacheSound(name);
    _sounds.emplace(name, std::make_unique<sf::Sound>(*buffer));
}

void MusicAudio::cacheAudio(const std::string &name)
{
    if (_music.find(name) != _music.end()) return;

    auto song = _resources.openUniquePtr<sf::Music>(name);
    _music.emplace(name, std::move(song));
}

} // namespace tt