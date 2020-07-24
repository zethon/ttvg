#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/filesystem.hpp>

#include <SFML/Graphics.hpp>

#include "../src/ResourceManager.h"
#include "../src/Background.h"
#include "../src/Vehicle.h"

#include "Test.h"

namespace tools = boost::test_tools;
namespace data = boost::unit_test::data;

BOOST_AUTO_TEST_SUITE(tt)
BOOST_AUTO_TEST_SUITE(vehicle)

BOOST_AUTO_TEST_CASE(fastVehicleTest)
{
    sf::Texture texture;
    texture.create(100, 100);

    tt::ResourceManager mgr{ boost::filesystem::path{} };
    auto bg = std::make_shared<tt::Background>("test", mgr, sf::Vector2f{ 5,5 });
    bg->setTexture(texture, true);
    bg->setScale(1.0f, 1.0f);

    sf::Texture vtext;
    vtext.create(10, 10);

    tt::Vehicle vehicle{ vtext, sf::Vector2i{10,10},  bg };
    vehicle.setSpeed(7.0f);

    tt::Path path;
    path.setRepeating(false);
    path.points().emplace_back(sf::Vector2f{ -1,0 });
    path.points().emplace_back(sf::Vector2f{ 2,0 });
    path.points().emplace_back(sf::Vector2f{ 2,10 });
    vehicle.setPath(path);

    vehicle.move();
    BOOST_TEST(vehicle.getGlobalBounds().left == 2.0f);

    vehicle.move();
    BOOST_TEST(vehicle.getGlobalBounds().left == 9.0f);

}

BOOST_AUTO_TEST_SUITE_END() // background
BOOST_AUTO_TEST_SUITE_END() // tt