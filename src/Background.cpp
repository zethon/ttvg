#include <fstream>

#include <fmt/core.h>

#include "ResourceManager.h"
#include "Background.h"

namespace nl = nlohmann;

namespace tt
{

void from_json(const nl::json& j, Zone& z)
{
    j.at("name").get_to(z.name);
    if (j.contains("description"))
    {
        j.at("description").get_to(z.description);
    }
    
    if (j.contains("transition"))
    {
        z.transition = j["transition"].get<Transition>();
    }
}

Background::Background(std::string_view name, ResourceManager& resmgr, const sf::Vector2f& tilesize)
    : _tilesize { tilesize },
      _mapname{ name }
{
    const std::string texturename = fmt::format("maps/{}.png", _mapname);
    _texture = resmgr.loadUniquePtr<sf::Texture>(texturename);
    if (_texture) setTexture(*_texture);

    const std::string jsonfile = 
        resmgr.getFilename(fmt::format("maps/{}.json", _mapname));

    std::ifstream file(jsonfile.c_str());
    if (file.is_open())
    {
        _json = std::make_unique<nl::json>();
        file >> *_json;
        file.close();
    }

    initZones();
}

void Background::initZones()
{
    if (!_json) return;
    if (!_json->at("zones").is_array()) return;

    for (const auto& item : (*_json)["zones"].items())
    {
        Zone zone = item.value().get<Zone>();
        for (const auto& c : item.value()["rects"].items())
        {
            std::string temp{ c.value().get<std::string>() };
            auto start = temp.begin();
            auto stop = temp.end();

            sf::FloatRect rect;
            bool result =
                phrase_parse(start, stop, FloatRectParser, x3::ascii::space, rect);

            if (result)
            {   
                zone.rect = rect;
                _zones.emplace(zone);
            }
        }
    }
}

sf::FloatRect Background::getWorldTileRect() const
{
    sf::Vector2f bound{ getGlobalBounds().width, getGlobalBounds().height };
    auto [width, height] = getTileFromGlobal(bound);
    return { 0, 0, width, height };
}

TileInfo Background::getTileInfo(const sf::Vector2f& v)
{
    // TODO: explore options of adding `RectConains` to 
    // the compare functor so that we can use `std::set::find`
    // to do this search, which should be faster
    for (const auto& zone : _zones)
    {
        if (RectContains(zone.rect, v))
        {
            TileInfo info;
            info.tile = v;
            info.type = TileType::ZONE;
            info.data = zone;
            return info;
        }
    }

    return {};
}

} // namespace tt