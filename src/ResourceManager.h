#pragma once
#include <filesystem>
#include <optional>

#include <SFML/Graphics/Font.hpp>

namespace tt
{

class ResourceManager
{
    std::filesystem::path _resourceFolder;

public:

    explicit ResourceManager(const std::filesystem::path& path);

    template<typename T>
    std::optional<T> load(const std::string& name)
    {
        auto filepath = _resourceFolder / name;
        T item;
        if (item.loadFromFile(filepath.string()))
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

};

} // namespace tt