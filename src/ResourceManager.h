#pragma once
#include <optional>
#include <iostream>

#include <boost/filesystem.hpp>

#include <SFML/Graphics/Font.hpp>

namespace tmx
{
    class Map;
}

namespace tt
{

using MapPtr = std::shared_ptr<tmx::Map>;

class ResourceManager
{
    boost::filesystem::path _resourceFolder;

public:

    explicit ResourceManager(const boost::filesystem::path& path);

    template<typename T>
    std::optional<T> load(const std::string& name)
    {
        auto filepath = _resourceFolder / name;
        if (T item; item.loadFromFile(filepath.string()))
        {
            return item;
        }

        return {};
    }

    template<typename T>
    std::shared_ptr<T> loadPtr(const std::string& name)
    {
        auto filepath = _resourceFolder / name;
        std::shared_ptr<T> item = std::make_shared<T>();
        if (item->loadFromFile(filepath.string()))
        {
            return item;
        }

        return {};
    }

    template<typename T>
    std::unique_ptr<T> openUniquePtr(const std::string& name)
    {
        auto filepath = _resourceFolder / name;
        std::unique_ptr<T> item = std::make_unique<T>();
        if (item->openFromFile(filepath.string()))
        {
            return item;
        }

        return {};
    }

    MapPtr loadMapPtr(const std::string& name);

};

} // namespace tt