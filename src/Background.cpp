#include <fstream>

#include <fmt/core.h>

#include "ResourceManager.h"
#include "Background.h"

namespace nl = nlohmann;

namespace tt
{

Background::Background(std::string_view name, ResourceManager& resmgr, sf::RenderTarget& target)
    : Background(name, resmgr, target, sf::Vector2f{ 16.f, 16.f })
{
}

Background::Background(std::string_view name, ResourceManager& resmgr, sf::RenderTarget& target, const sf::Vector2f& tilesize)
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

    initBackground(target);
    initZones();

    // tilesize must come from either the constructor or
    // the json configuration
    assert(_tilesize.x > 0 && _tilesize.y > 0);
}

void Background::initBackground(const sf::RenderTarget& target)
{
    if (!_json) return;
    if (!_json->contains("background")) return;
    if (!_json->at("background").is_object()) return;

    const auto bg = _json->at("background");
    if (bg.contains("scale"))
    {
        sf::Vector2f scale{ 1.f, 1.f };

        if (bg.at("scale").is_string()
            && bg.at("scale") == "auto")
        {
            scale = sf::Vector2f{
                static_cast<float>(target.getSize().x) / static_cast<float>(getTexture()->getSize().x),
                static_cast<float>(target.getSize().y) / static_cast<float>(getTexture()->getSize().y) };
        }
        else if (bg.at("scale").is_object())
        {
            scale = bg["scale"].get<sf::Vector2f>();
            assert(scale.x > 0.f);
            assert(scale.y > 0.f);
        }

        this->setScale(scale);
    }

    if (bg.contains("tiles"))
    {
        _tilesize = bg["tiles"].get<sf::Vector2f>();
    }

    if (bg.contains("position"))
    {
        this->setPosition(bg["position"].get<sf::Vector2f>());
    }

    if (bg.contains("camera") && bg.at("camera").is_string())
    {
        if (bg["camera"].get<std::string>() == "follow")
        {
            _cameraType = CameraType::FOLLOW;
        }
        else
        {
            _cameraType = CameraType::FIXED;
        }
    }
}

void Background::initZones()
{
    if (!_json) return;
    if (!_json->contains("zones")) return;
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