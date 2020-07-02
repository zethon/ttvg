#include <fstream>

#include <boost/filesystem.hpp>

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#include "ResourceManager.h"
#include "Background.h"

namespace tt
{

Background::Background(std::string_view name, ResourceManager& resmgr)
{
    using namespace nlohmann;

    const std::string texturename = fmt::format("maps/{}.png", name);
    _texture = resmgr.loadUniquePtr<sf::Texture>(texturename);
    setTexture(*_texture);

    const std::string jsonfile = 
        resmgr.getFilename(fmt::format("maps/{}.json", name));

    std::ifstream file(jsonfile.c_str());
    json j;
    file >> j;
    file.close();

    if (!j.at("zones").is_array()) return;

    for (const auto& item : j["zones"].items())
    {
        std::cout << "name: " << item.value()["name"] << '\n';
        for (const auto& c: item.value()["rects"].items())
        {
            std::cout << "y: " << c.value() << '\n';
        }
    }
}

sf::FloatRect Background::getWorldRect() const
{
    return { 0, 0, getRightBoundary(), getBottomBoundary() };
}

float Background::getRightBoundary() const
{
    return getTextureRect().width * this->getScale().x;
}

float Background::getBottomBoundary() const
{
    return getTextureRect().height * this->getScale().y;;
}

} // namespace tt