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

    std::optional<sf::Font> loadFont(const std::string& name);
};

} // namespace tt