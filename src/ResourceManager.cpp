#include <tmxlite/Map.hpp>

#include "ResourceManager.h"

namespace tt
{

ResourceManager::ResourceManager(const boost::filesystem::path& path)
    : _resourceFolder{ path }
{}

MapPtr ResourceManager::loadMapPtr(const std::string& name)
{
    auto filepath = _resourceFolder / name;
    auto item = std::make_shared<tmx::Map>();
    if (item->load(filepath.string()))
    {
        return item;
    }

    return {};
}

} // namespace tt