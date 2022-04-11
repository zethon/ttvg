#include "ResourceManager.h"
#include "AudioService.h"

namespace tt
{

namespace
{

int AudioLocator_cacheMusic(lua_State* L)
{
    const auto audioid = lua_tostring(L, 1);
    tt::AudioLocator::music()->cacheAudio(audioid);
    return 0;
}

int AudioLocator_playMusic(lua_State* L)
{
    const auto audioid = lua_tostring(L, 1);
    tt::AudioLocator::music()->play(audioid);
    return 0;
}

int AudioLocator_stopMusic(lua_State* L)
{
    const auto audioid = lua_tostring(L, 1);
    tt::AudioLocator::music()->stop(audioid);
    return 0;
}

int AudioLocator_pauseMusic(lua_State* L)
{
    const auto audioid = lua_tostring(L, 1);
    tt::AudioLocator::music()->pause(audioid);
    return 0;
}

int AudioLocator_cacheSound(lua_State* L)
{
    const auto audioid = lua_tostring(L, 1);
    tt::AudioLocator::sound()->cacheAudio(audioid);
    return 0;
}

int AudioLocator_playSound(lua_State* L)
{
    const auto audioid = lua_tostring(L, 1);
    tt::AudioLocator::sound()->play(audioid);
    return 0;
}

int AudioLocator_stopSound(lua_State* L)
{
    const auto audioid = lua_tostring(L, 1);
    tt::AudioLocator::sound()->stop(audioid);
    return 0;
}

int AudioLocator_pauseSound(lua_State* L)
{
    const auto audioid = lua_tostring(L, 1);
    tt::AudioLocator::sound()->pause(audioid);
    return 0;
}

} // anonymous namespace

const struct luaL_Reg AudioLocator::MusicLuaMethods[] =
{
    {"cache", AudioLocator_cacheMusic},
    {"play", AudioLocator_playMusic},
    {"stop", AudioLocator_stopMusic},
    {"pause", AudioLocator_pauseMusic},
    {nullptr, nullptr}
};

const struct luaL_Reg AudioLocator::SoundLuaMethods[] =
{
    {"cache", AudioLocator_cacheSound},
    {"play", AudioLocator_playSound},
    {"stop", AudioLocator_stopSound},
    {"pause", AudioLocator_pauseSound},
    {nullptr, nullptr}
};

void SfxAudio::cacheAudio(const std::string& name)
{
    auto buffer = _resources.cacheSound(name);
    auto sound = std::make_unique<sf::Sound>(*buffer);
    sound->setVolume(this->_volume);
    _sounds.emplace(name, std::move(sound));
}

void MusicAudio::cacheAudio(const std::string &name)
{
    if (_music.find(name) != _music.end()) return;

    auto song = _resources.openUniquePtr<sf::Music>(name);
    song->setVolume(_volume);
    _music.emplace(name, std::move(song));
}

} // namespace tt