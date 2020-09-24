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

// --run_test=tt/items/loadItemTest
BOOST_AUTO_TEST_CASE(loadItemTest)
{
    const auto resfolder = fmt::format("{}/resources", TT_SRC_DIRECTORY_);
    tt::ResourceManager resmgr{ resfolder };

    tt::ItemFactory itemf{ resmgr };

    auto item = itemf.createItem("bag", {});
    BOOST_TEST(item->getID() == "bag");
    BOOST_TEST(item->getName() == "Bag");
    BOOST_TEST(item->isObtainable());
    
    item = itemf.createItem("nissan-truck", {});
    BOOST_TEST(item->getID() == "nissan-truck");
    BOOST_TEST(item->getName() == "Nissan Vagina");
    BOOST_TEST(!item->isObtainable());
}

const auto bagjson = R"x({
    "name":         "Bag of Crack",
    "description":  "This is good for your blood pressure.",
    "obtainable":   false,
    "attributes":   { }
})x";

const auto bagscenejson = R"x({
"background":{"tiles":{ "x": 16, "y": 16 }},
"player":{"start": { "x": 35, "y": 35 }},
"items":
{
    "bag": 
    {
        "instances":
        [    
            { "x": 5, "y": 5 },
            { "x":"random", "y":"random"}
        ]
    }
}})x";

// --run_test=tt/items/itemFlagsTest
BOOST_AUTO_TEST_CASE(itemFlagsTest)
{
    const auto path{ tt::tempFolder() };
    const auto resfolder{ path / "resources" };
    const auto itemspath{ resfolder / "items" };

    // copy the source's items folder to the test folder
    const auto itemsrc{ fmt::format("{}/resources/items", TT_SRC_DIRECTORY_) };

    // we will use the bag json file for testing
    copyFile((fs::path{itemsrc} / "bag.png"), itemspath / "bag.png");
    writeFile((fs::path{itemspath} / "bag.json").string(), bagjson);
    writeFile((fs::path{ itemspath } / "bag.json").string(), bagjson);

    tt::ResourceManager resmgr{ resfolder };
    tt::ItemFactory itemf{ resmgr };

    auto item = itemf.createItem("bag", {});
    BOOST_TEST(item->getID() == "bag");
    BOOST_TEST(item->getName() == "Bag of Crack");
    BOOST_TEST(!item->isObtainable());
}

BOOST_AUTO_TEST_SUITE_END() // items
BOOST_AUTO_TEST_SUITE_END() // tt