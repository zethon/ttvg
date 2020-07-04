#include <fstream>

#include <boost/filesystem.hpp>

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#include "TTUtils.h"
#include "ResourceManager.h"
#include "Background.h"

namespace tt
{

Background::Background(std::string_view name, ResourceManager& resmgr, const sf::Vector2i& tilesize)
    : _tilesize { tilesize }
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
        for (const auto& c: item.value()["rects"].items())
        {
            std::string temp { c.value().get<std::string>() };
            auto start = temp.begin();
            auto stop = temp.end();

            sf::FloatRect rect;
            bool result =
                phrase_parse(start, stop, FloatRectParser, x3::ascii::space, rect);

            if (result)
            {
                _zones.emplace_back(item.value()["name"].get<std::string>(), rect);
            }
            
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

std::string Background::zoneName(const sf::Vector2f& v)
{
    for (const auto& [name, rect] : _zones)
    {
        if (rect.contains(v))
        {
            return name;
        }
    }

    return std::string();
}

} // namespace tt