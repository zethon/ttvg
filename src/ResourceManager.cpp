#include "ResourceManager.h"

namespace tt
{

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


} // namespace tt