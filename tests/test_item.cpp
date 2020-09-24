#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/filesystem.hpp>

#include <fmt/format.h>

#include <SFML/Graphics.hpp>

#include "../src/ResourceManager.h"
#include "../src/GameScreen.h"
#include "../src/TTLua.h"
#include "../src/Scenes/Scene.h"
#include "../src/Item.h"
#include "../src/ItemFactory.h"

#include "Test.h"

namespace tools = boost::test_tools;
namespace data = boost::unit_test::data;
namespace fs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(tt)
BOOST_AUTO_TEST_SUITE(items)

class GameScreenStub {};

struct TestHarness
{
    tt::ResourceManager _resources;
    tt::NullWindow  _window;
    tt::PlayerPtr   _player;
    lua_State* _lua;
    std::shared_ptr<ItemFactory> _itemFactory;

    TestHarness(const std::string& resfolder)
        : _resources{ resfolder }
    {
        _lua = luaL_newstate();

        sf::Texture texture;
        texture.create(100, 100);
        _player = std::make_shared<tt::Player>(texture, sf::Vector2i{ 10,10 });

        _itemFactory = std::make_shared<ItemFactory>(_resources);

        GameScreenStub stub;
        tt::initLua(_lua, stub, static_cast<void*>(_itemFactory.get()));
    }

    tt::SceneSetup setup()
    {
        return tt::SceneSetup 
        {
            _resources,
            _window,
            _player,
            _lua,
            _itemFactory 
        };
    }
};

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

    TestHarness harness{ (path / "resources").string() };

}

BOOST_AUTO_TEST_SUITE_END() // items
BOOST_AUTO_TEST_SUITE_END() // tt