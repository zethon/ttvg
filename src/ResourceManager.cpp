#include "ResourceManager.h"

namespace tt
{

ResourceManager::ResourceManager(const boost::filesystem::path& path)
    : _resourceFolder{ path }
{
}

////////////////////////////////////////////////////////////
bool ResourceManager::cacheTexture(const std::string& name)
{
    const auto filename = getFilename(name);

    if (sf::Texture texture;
        texture.loadFromFile(filename))
    {
        _textcache.emplace(name, std::move(texture));
        return true;
    }

    return false;
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
std::string ResourceManager::getFilename(const std::string& name)
{
    auto filepath = _resourceFolder / name;
    return filepath.string();
}


} // namespace tt