#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/filesystem.hpp>

#include <nlohmann/json.hpp>
#include <fmt/format.h>

#include <SFML/Graphics.hpp>

#include "../src/Screens/GameScreen.h"

#include "../src/ResourceManager.h"
#include "../src/TTLua.h"
#include "../src/Scenes/Scene.h"
#include "../src/Item.h"
#include "../src/ItemFactory.h"
#include "../src/AudioService.h"

#include "Test.h"

namespace tools = boost::test_tools;
namespace data = boost::unit_test::data;
namespace fs = boost::filesystem;
namespace nl = nlohmann;

BOOST_AUTO_TEST_SUITE(tt)
BOOST_AUTO_TEST_SUITE(items)

class GameScreenStub {};

// TODO: there were problems taking this out of test_lua.cpp and putting
// it into a shared header. Problems I don't feel like dealing with right
// now at 5:30am, so I'm going to dupliacte this helper class here
struct TestHarness
{
    tt::ResourceManager _resources;
    tt::NullWindow  _window;
    tt::PlayerPtr   _player;
    lua_State* _lua;
    std::shared_ptr<ItemFactory> _itemFactory;
    sf::Texture _playerTexture;

    TestHarness(const std::string& resfolder)
        : _resources{ resfolder, nullptr }
    {
        tt::AudioLocator::setMusic(std::make_shared<tt::NullAudio>());
        tt::AudioLocator::setSound(std::make_shared<tt::NullAudio>());

        _lua = luaL_newstate();
        _playerTexture.create(100, 100);

        tt::ItemInfo playerObjInfo;
        playerObjInfo.size = sf::Vector2u{ 10, 10 };
        playerObjInfo.texture = &_playerTexture;

        _player = std::make_shared<tt::Player>(playerObjInfo, ItemInstanceInfo{});

        _itemFactory = std::make_shared<ItemFactory>(_resources);

        GameScreenStub stub;
        tt::initLua(_lua, stub, static_cast<void*>(_itemFactory.get()), nullptr);
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
    tt::ResourceManager resmgr{ resfolder, nullptr };

    tt::ItemFactory itemf{ resmgr };

    ItemInstanceInfo ii1;
    ii1.objid = "bag";
    auto item = itemf.createItem(ii1);
    BOOST_TEST(item->getID() == "bag");
    BOOST_TEST(item->getName() == "Bag");
    BOOST_TEST(item->obtainable());
    
    ItemInstanceInfo ii2;
    ii2.objid = "nissan-truck";
    item = itemf.createItem(ii2);
    BOOST_TEST(item->getID() == "nissan-truck");
    BOOST_TEST(item->getName() == "Nissan Vagina");
    BOOST_TEST(!item->obtainable());
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
        "onSelect": "bag_onSelect",
        "instances":
        [    
            { "x": 5, "y": 5 },
            { "x":"random", "y":"random", "onSelect": "special_bagPickup" },
            { "x":"random", "y":"random", "onSelect": "" }
        ]
    }
}})x";

// --run_test=tt/items/itemRandomPlacementTest
BOOST_AUTO_TEST_CASE(itemRandomPlacementTest)
{
    const auto path{ tt::tempFolder() };
    const auto resfolder{ path / "resources" };
    const auto itemspath{ resfolder / "items" };
    const auto mappath{ resfolder / "maps" };

    // copy the source's items folder to the test folder
    const auto itemsrc{ fmt::format("{}/resources/items", TT_SRC_DIRECTORY_) };

    // we will use the bag json file for testing
    copyFile((fs::path{itemsrc} / "bag.png"), itemspath / "bag.png");
    writeFile((itemspath / "bag.json").string(), bagjson);
    writeFile((mappath / "scene1.json").string(), bagscenejson);

    TestHarness harness{ resfolder.string() };
    auto scene = std::make_shared<tt::Scene>("scene1", harness.setup());
    scene->init();

    BOOST_TEST(scene->items().size() == 3);

    const auto expxy = scene->background()->getGlobalFromTile(5.f, 5.f);
    
    const auto& firstItem = scene->items().at(0);
    BOOST_TEST(firstItem->getPosition().x == expxy.x, tools::tolerance(0.001));
    BOOST_TEST(firstItem->getPosition().y == expxy.y, tools::tolerance(0.001));
    BOOST_TEST(firstItem->callbacks().onSelect.has_value());
    BOOST_TEST(*(firstItem->callbacks().onSelect) == "bag_onSelect");

    const auto& seconditem = scene->items().at(1);
    BOOST_TEST(seconditem->callbacks().onSelect.has_value());
    BOOST_TEST(*(seconditem->callbacks().onSelect) == "special_bagPickup");

    const auto& thirditem = scene->items().at(2);
    BOOST_TEST(thirditem->callbacks().onSelect.has_value());
    BOOST_TEST(thirditem->callbacks().onSelect->empty());
}

BOOST_AUTO_TEST_SUITE_END() // items

BOOST_AUTO_TEST_SUITE(gameobjects)

const auto sevanjson = R"x({ "name": "sevan2",
"description": "this is the description",
"size": { "x": 64, "y": 64 },
"scale": { "x": 1.0, "y": 1.25 },
"states":
    [
        { "id":"up","source":{ "x":0, "y":0 },"count":2 },
        { "id":"down", "source":{ "x":0, "y":128 },"count":2 }
    ]
})x";

// --run_test=tt/gameobjects/gameObjectLoadTest
BOOST_AUTO_TEST_CASE(gameObjectLoadTest)
{
    nl::json jsondata = nl::json::parse(sevanjson, nullptr, false);
    BOOST_TEST(!jsondata.is_discarded());

    const auto object = jsondata.get<tt::ItemInfo>();
    BOOST_TEST(object.name == "sevan2");
    BOOST_TEST(object.states.size() == 2);

    BOOST_TEST((object.states.find("up") != object.states.end()));
    BOOST_TEST((object.states.find("down") != object.states.end()));

    BOOST_TEST(object.states.at("up").source.y == 0);
    BOOST_TEST(object.states.at("down").source.y == 128);
}

BOOST_AUTO_TEST_SUITE_END() // gameobjects

BOOST_AUTO_TEST_SUITE_END() // tt
