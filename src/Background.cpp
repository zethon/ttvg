#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#include "TTUtils.h"
#include "ResourceManager.h"
#include "Background.h"

namespace x3 = boost::spirit::x3;

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

    const auto realCSVParser
        = x3::rule<class realCSVParserID, sf::FloatRect>{}
        = (x3::float_ >> ',' >> x3::float_ >> ',' >> x3::float_ >> ',' >> x3::float_)
            [([](auto& ctx)
                {
                    auto& attr = x3::_attr(ctx);
                    using boost::fusion::at_c;

                    auto width = at_c<2>(attr) - at_c<0>(attr);
                    auto height = at_c<3>(attr) - at_c<1>(attr);
                    x3::_val(ctx)
                        = sf::FloatRect{ at_c<0>(attr), at_c<1>(attr), width, height };
                })
            ];

    for (const auto& item : j["zones"].items())
    {
        for (const auto& c: item.value()["rects"].items())
        {
            std::string temp { c.value().get<std::string>() };
            auto start = temp.begin();
            auto stop = temp.end();

            sf::FloatRect rect;
            bool result =
                parse(start, stop, realCSVParser, rect);

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