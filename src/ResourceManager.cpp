#include <fmt/core.h>

#include "ResourceManager.h"
#include "TTLua.h"

namespace tt
{

ResourceManager* ResourceManager::l_get(lua_State * L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, RESOURCEMGR_LUA_IDX);
    int type = lua_type(L, -1);
    if (type != LUA_TLIGHTUSERDATA)
    {
        return nullptr;
    }

    ResourceManager* state = static_cast<ResourceManager*>(lua_touserdata(L, -1));
    lua_pop(L, 1); // -1
    if (!state)
    {
        return nullptr;
    }
    return state;
}

ResourceManager::ResourceManager(const boost::filesystem::path& path)
    : _resourceFolder{ path }
{
}

////////////////////////////////////////////////////////////
sf::Texture* ResourceManager::cacheTexture(const std::string& name)
{
    if (auto temp = getTexture(name);
        temp != nullptr)
    {
        return temp;
    }

    const auto filename = getFilename(name);
    if (sf::Texture texture;
        texture.loadFromFile(filename))
    {
        auto retval = _textcache.emplace(name, std::move(texture));
        return &(retval.first->second);
    }

    return nullptr;
}

////////////////////////////////////////////////////////////
sf::Texture* ResourceManager::getTexture(const std::string& name)
{
    if (_textcache.find(name) != _textcache.end())
    {
        return &(_textcache[name]);
    }

    return nullptr;
}

////////////////////////////////////////////////////////////
sf::SoundBuffer* ResourceManager::cacheSound(const std::string & name)
{
    if (auto temp = getSound(name);
        temp != nullptr)
    {
        return temp;
    }

    const auto filename = getFilename(name);
    if (sf::SoundBuffer sound;
        sound.loadFromFile(filename))
    {
        auto retval = _soundcache.emplace(name, std::move(sound));
        return &(retval.first->second);
    }

    return nullptr;
}

////////////////////////////////////////////////////////////
sf::SoundBuffer* ResourceManager::getSound(const std::string & name)
{
    if (_soundcache.find(name) != _soundcache.end())
    {
        return &(_soundcache[name]);
    }

    return nullptr;
}

void ResourceManager::clearCaches()
{
    _textcache.clear();
    _soundcache.clear();
}

////////////////////////////////////////////////////////////
std::string ResourceManager::getFilename(const std::string& name)
{
    auto filepath = _resourceFolder / name;
    return filepath.string();
}

////////////////////////////////////////////////////////////
std::optional<nl::json> ResourceManager::getJson(const std::string& name)
{
    const std::string jsonfile =  this->getFilename(name);

    std::ifstream file(jsonfile.c_str());
    if (file.is_open())
    {
        nl::json retval;
        file >> retval;
        file.close();
        return retval;
    }

    return {};
}

} // namespace tt