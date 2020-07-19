#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <SFML/Graphics.hpp>

#include "../src/ResourceManager.h"
#include "../src/Background.h"

namespace tools = boost::test_tools;

BOOST_AUTO_TEST_SUITE(tt)
BOOST_AUTO_TEST_SUITE(background)

BOOST_AUTO_TEST_CASE(rectTest, *boost::unit_test::tolerance(0.001))
{
    sf::Texture texture;
    texture.create(1000, 1000);

    tt::ResourceManager mgr{ boost::filesystem::path{} };
    tt::Background bg{ "test", mgr, sf::Vector2i { 20, 20} };
    bg.setTexture(texture, true);
    bg.setScale(1.5f, 1.5f);
}

BOOST_AUTO_TEST_SUITE_END() // background
BOOST_AUTO_TEST_SUITE_END() // tt