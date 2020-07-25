#pragma once
#include <optional>
#include <iostream>

#include <boost/filesystem.hpp>

#include <nlohmann/json_fwd.hpp>

#include <SFML/Graphics/Font.hpp>

namespace tt
{

class ResourceManager
{
    using TextureCache = std::map<std::string, sf::Texture>;

    boost::filesystem::path     _resourceFolder;
    TextureCache                _textcache;

public:
    explicit ResourceManager(const boost::filesystem::path& path);

    /// \brief Loads a texture into the cache
    ///
    /// \param name The relative path of the texture from the
    ///             resource folder (e.g. "items/sax.png").
    ///
    /// \ see getTexture
    ///
    /// \return Pointer to the object in the container, or null
    ///
    sf::Texture* cacheTexture(const std::string& name);

    /// \brief Returns a pointer to the texture
    ///
    /// \param name The relative path of the texture
    ///             (e.g. "items/sax.png").
    ///
    /// \ see cacheTexture
    ///
    /// \return A pointer to the texture or NULL
    ///
    sf::Texture* getTexture(const std::string& name);
    void clearTextureCache() { _textcache.clear(); }

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
    std::unique_ptr<T> loadUniquePtr(const std::string& name)
    {
        auto filepath = _resourceFolder / name;
        std::unique_ptr<T> item = std::make_unique<T>();
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

    std::string getFilename(const std::string& name);
};

} // namespace tt