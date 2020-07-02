#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/spirit/home/x3.hpp>

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
        = x3::rule<class realCSVParserID, std::vector<float>, true> { "realCSVParser" }
        = *(x3::lit('"')) >> x3::float_ >> x3::repeat(3)[',' >> x3::float_] >> *(x3::lit('"'));

    for (const auto& item : j["zones"].items())
    {
        std::cout << "name: " << item.value()["name"] << '\n';
        for (const auto& c: item.value()["rects"].items())
        {
            sf::FloatRect newrect;

            std::vector<float> numbers;
            std::string temp { c.value().get<std::string>() };
            auto start = temp.begin();
            auto stop = temp.end();

            bool result =
                phrase_parse(start, stop, realCSVParser, x3::ascii::space, numbers);

            std::cout << "y: [" << c.value() << "] :" << "bob" << '\n';
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