#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/filesystem.hpp>

#include <fmt/format.h>

#include <SFML/Graphics.hpp>

#include "../src/Item.h"
#include "../src/ItemFactory.h"

#include "Test.h"

namespace tools = boost::test_tools;
namespace data = boost::unit_test::data;
namespace fs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(tt)
BOOST_AUTO_TEST_SUITE(items)

BOOST_AUTO_TEST_CASE(loadItemTest)
{
    const auto resfolder = fmt::format("{}/resources", TT_SRC_DIRECTORY_);
    tt::ResourceManager resmgr{ resfolder };

    tt::ItemFactory itemf{ resmgr };
    auto item = itemf.createItem("bag", sf::Vector2f{1.f, 1.f}, {});

    BOOST_TEST(item->getID() == "bag");
    BOOST_TEST(item->getName() == "Bag");
    BOOST_TEST(item->isObtainable());
}

BOOST_AUTO_TEST_SUITE_END() // items
BOOST_AUTO_TEST_SUITE_END() // tt