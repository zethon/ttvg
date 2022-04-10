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

    sf::Texture* cacheTexture(const std::string& name);

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
    std::optional<nl::json> getJson(const std::string& name);

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