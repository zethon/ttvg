#pragma once
#include <optional>
#include <iostream>

#include <lua.hpp>

#include <boost/filesystem.hpp>

#include <nlohmann/json.hpp>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio.hpp>

#include "Settings.h"

namespace nl = nlohmann;

namespace tt
{

class ResourceManager
{
    using TextureCache = std::map<std::string, sf::Texture>;
    using SoundCache = std::map<std::string, sf::SoundBuffer>;

    boost::filesystem::path     _resourceFolder;
    TextureCache                _textcache;
    SoundCache                  _soundcache;

    amb::SettingsPtr            _config;

public:
    explicit ResourceManager(const boost::filesystem::path& path,
                             const amb::SettingsPtr& settings);

    static ResourceManager* l_get(lua_State* L);

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

    sf::SoundBuffer* cacheSound(const std::string& name);
    sf::SoundBuffer* getSound(const std::string& name);
    void clearSoundCache() { _soundcache.clear();  }

    void clearCaches();

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

    /// \brief Return a loaded JSON file
    ///
    /// \param name Filename and relative path of the JSON file
    ///             (e.g. "maps/tucson.json").
    ///
    /// \return An optional with the loaded JSON object if loaded
    ///
    std::optional<nl::json> getJson(const std::string& name);

    /// \brief Return a copy of the game settings
    amb::SettingsPtr settings() const
    {
        return _config;
    }

    amb::SettingsPtr settings()
    {
        return _config;
    }
};

} // namespace tt