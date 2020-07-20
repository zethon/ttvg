#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <SFML/Graphics.hpp>

#include "../src/ResourceManager.h"
#include "../src/Background.h"

namespace tools = boost::test_tools;
namespace data = boost::unit_test::data;

using TilePair = std::tuple<sf::Vector2f, sf::Vector2f>;
using TilePairs = std::vector<TilePair>;

namespace std
{

template<typename T>
std::ostream& operator<<(std::ostream& out, const sf::Vector2<T> item)
{
    auto[x, y] = item;
    out << "{ x=" << x
        << " y=" << y
        << " }";

    return out;
}

std::ostream& operator<<(std::ostream& out, const TilePair& item)
{
    out << std::get<0>(item) << ',' << std::get<1>(item);
    return out;
}

std::ostream& operator<<(std::ostream& out, const TilePairs& item)
{
    for (const auto& i : item)
    {
        out << i << ' ';
    }

    return out;
}

}

BOOST_AUTO_TEST_SUITE(tt)
BOOST_AUTO_TEST_SUITE(bg)

// 0 - texture size
// 1 - tile size
// 2 - scale (x and y)
// 3 - array of tile and global coords to test
std::tuple<sf::Vector2u, sf::Vector2f, sf::Vector2f, TilePairs> rectTestData[] = 
{
    {
        { 100, 100 },
        { 2.0f, 2.0f },
        { 1.0f, 1.0f},
        {
            { { 0.0f, 0.0f}, { 0.0f, 0.0f } },
            { { 1.0f, 0.0f}, { 2.0f, 0.0f } },
            { { 0.0f, 1.0f}, { 0.0f, 2.0f } },
            { { 1.0f, 1.0f}, { 2.0f, 2.0f } },
        }   
    },
    {
        { 1000, 1000 },
        { 20.0f, 20.0f },
        { 1.5f, 1.5f},
        {
            { { 0.0f, 0.0f}, { 0.0f, 0.0f } },
            { { 1.0f, 0.0f}, { 30.0f, 0.0f } },
            { { 0.0f, 1.0f}, { 0.0f, 30.0f } },
            { { 1.0f, 1.0f}, { 30.0f, 30.0f } },
        }   
    }
};

// --run_test=tt/bg/testRect
BOOST_DATA_TEST_CASE(testRect, data::make(rectTestData), textSize, tileSize, bgScale, testdata)
{
    // sf::Texture texture;
    // texture.create(textSize.x, textSize.y);

    // tt::ResourceManager mgr{ boost::filesystem::path{} };
    // tt::Background bg{ "test", mgr, tileSize };
    // bg.setTexture(texture, true);
    // bg.setScale(bgScale);

    // for (const auto&[tile, global] : testdata)
    // {
    //     const auto calc = bg.getGlobalFromTile(tile);
    //     BOOST_TEST(calc.x == global.x, tools::tolerance(float(0.01))); 
    // }
}

BOOST_AUTO_TEST_SUITE_END() // background
BOOST_AUTO_TEST_SUITE_END() // tt