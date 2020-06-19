#include "ResourceManager.h"

namespace tt
{

ResourceManager::ResourceManager(const std::filesystem::path& path)
    : _resourceFolder{ path }
{}

std::optional<sf::Font> ResourceManager::loadFont(const std::string& name)
{
    auto filepath = _resourceFolder / name;
    sf::Font font;
    if (font.loadFromFile(filepath.string()))
    {
        return font;
    }

    return {};
}

} // namespace tt