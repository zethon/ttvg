#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <SFML/Graphics.hpp>

#include "../src/ResourceManager.h"
#include "../src/Background.h"

#include "Test.h"

namespace tools = boost::test_tools;
namespace data = boost::unit_test::data;

using TilePair = std::tuple<sf::Vector2f, sf::Vector2f>;
using TilePairs = std::vector<TilePair>;

namespace std
{

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
std::tuple<sf::Vector2u, sf::Vector2f, sf::Vector2f, TilePairs> globalFromTileData[] =
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
            { { -1.0f, 0.0f}, { -2.0f, 0.0f } }
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
BOOST_DATA_TEST_CASE(globalFromTileTest, data::make(globalFromTileData), textSize, tileSize, bgScale, testdata)
{
    sf::Texture texture;
    texture.create(textSize.x, textSize.y);

    tt::NullWindow window;

    tt::ResourceManager mgr{ boost::filesystem::path{} };
    tt::Background bg{ "test", mgr, window, tileSize };
    bg.setTexture(texture, true);
    bg.setScale(bgScale);

    for (const auto&[tile, global] : testdata)
    {
        const auto g = bg.getGlobalFromTile(tile);
        BOOST_TEST(g.x == global.x, tools::tolerance(float(0.01)));
        BOOST_TEST(g.y == global.y, tools::tolerance(float(0.01)));

        // round trip testing
        const auto testtile = bg.getTileFromGlobal(g);
        BOOST_TEST(testtile.x == tile.x, tools::tolerance(float(0.01)));
        BOOST_TEST(testtile.y == tile.y, tools::tolerance(float(0.01)));
    }
}

// 0 - texture size
// 1 - tile size
// 2 - scale (x and y)
// 3 - array of tile and global coords to test
std::tuple<sf::Vector2u, sf::Vector2f, sf::Vector2f, TilePairs> globalCenterFromTileData[] =
{
    {
        { 100, 100 },
        { 10.0f, 10.0f },
        { 1.5f, 1.5f},
        {
            { { 0.0f, 0.0f}, { 7.5f, 7.5f } },
        }
    }
};

// --run_test=tt/bg/globalCenterFromTileTest
BOOST_DATA_TEST_CASE(globalCenterFromTileTest, data::make(globalCenterFromTileData), textSize, tileSize, bgScale, testdata)
{
    sf::Texture texture;
    texture.create(textSize.x, textSize.y);

    tt::NullWindow window;

    tt::ResourceManager mgr{ boost::filesystem::path{} };
    tt::Background bg{ "test", mgr, window, tileSize };
    bg.setTexture(texture, true);
    bg.setScale(bgScale);

    for (const auto&[tile, global] : testdata)
    {
        const auto gcenter = bg.getGlobalCenterFromTile(tile);
        BOOST_TEST(gcenter.x == global.x, tools::tolerance(float(0.01)));
        BOOST_TEST(gcenter.y == global.y, tools::tolerance(float(0.01)));

        // round trip testing
        const auto testtile = bg.getTileFromGlobal(gcenter);
        BOOST_TEST(testtile.x == tile.x, tools::tolerance(float(0.01)));
        BOOST_TEST(testtile.y == tile.y, tools::tolerance(float(0.01)));
    }
}

BOOST_AUTO_TEST_SUITE_END() // background
BOOST_AUTO_TEST_SUITE_END() // tt