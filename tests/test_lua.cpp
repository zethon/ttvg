#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <fmt/format.h>

#include <SFML/Graphics.hpp>

#include "../src/Screens/GameScreen.h"

#include "../src/ResourceManager.h"
#include "../src/TTLua.h"
#include "../src/Scenes/Scene.h"
#include "../src/GameWorld.h"
#include "../src/AudioService.h"

#include "Test.h"

namespace tools = boost::test_tools;
namespace data = boost::unit_test::data;
namespace fs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(tt)
BOOST_AUTO_TEST_SUITE(lua)

class GameScreenStub {};

struct TestHarness
{
    tt::ResourceManager _resources;
    tt::NullWindow  _window;
    tt::PlayerPtr   _player;
    lua_State* _lua;
    std::shared_ptr<ItemFactory> _itemFactory;
    sf::Texture _playerTexture;
    tt::ItemInfo _playerObjInfo;
    tt::GameCalendarPtr _calendar;
    std::shared_ptr<tt::Hud> _hud;

    TestHarness(const std::string& resfolder)
        : _resources{ resfolder, nullptr }
    {
        tt::AudioLocator::setMusic(std::make_shared<tt::NullAudio>());
        tt::AudioLocator::setSound(std::make_shared<tt::NullAudio>());

        _lua = luaL_newstate();
        _playerTexture.create(100, 100);

        _playerObjInfo.size = sf::Vector2u{ 10, 10 };
        _playerObjInfo.framecount = 9;
        _playerObjInfo.states.emplace("up", ItemState{ "up", sf::Vector2i{0,0}, 9, 55, HitBox{0,0,64,64} });
        _playerObjInfo.states.emplace("left", ItemState{ "left", sf::Vector2i{0,1}, 9, 55, HitBox{0,0,64,64} });
        _playerObjInfo.states.emplace("down", ItemState{ "down", sf::Vector2i{0,2}, 9, 55, HitBox{0,0,64,64} });
        _playerObjInfo.states.emplace("right", ItemState{ "right", sf::Vector2i{0,3}, 9, 55, HitBox{0,0,64,64} });
        _playerObjInfo.texture = &_playerTexture;
        _playerObjInfo.defaultState = "up";

        _player = std::make_shared<tt::Player>(_playerObjInfo, ItemInstanceInfo{});
        _calendar = std::make_shared<tt::GameWorld>();
        _hud = std::make_shared<tt::Hud>(_resources, _window, _calendar);
        _itemFactory = std::make_shared<ItemFactory>(_resources);

        GameScreenStub stub;
        tt::initLua(_lua, stub,
            static_cast<void*>(_itemFactory.get()),
            static_cast<void*>(&_resources));
    }

    tt::SceneSetup setup()
    {
        return tt::SceneSetup 
        {
            _resources,
            _window,
            _player,
            _lua,
            _itemFactory,
            _calendar,
            _hud
        };
    }
};

struct SceneStub
{
    static constexpr auto CLASS_NAME = "Scene";
    std::string _name;
    std::string name() const { return _name; }
};

constexpr auto file1 = R"lua(
function onInit()
return "mylua1"
end
)lua";

constexpr auto file2 = R"lua(
function onInit()
return "mylua2"
end
)lua";

BOOST_AUTO_TEST_CASE(loadTestCase)
{
    GameScreenStub stub;
    lua_State* lua = luaL_newstate();

    tt::initLua(lua, stub, nullptr, nullptr);
    BOOST_TEST_REQUIRE(lua != nullptr);

    auto path = tt::tempFolder();
    const auto luafile1 = (path / "file1.lua").string();
    const auto luafile2 = (path / "file2.lua").string();

    writeFile(luafile1, file1);
    writeFile(luafile2, file2);

    SceneStub s1{ "scene1" };
    auto s1idx = tt::registerObject(lua, s1);

    SceneStub s2{ "scene2" };
    auto s2idx = tt::registerObject(lua, s2);

    BOOST_TEST(tt::loadSceneLuaFile(s1, luafile1, lua));
    BOOST_TEST(tt::loadSceneLuaFile(s2, luafile2, lua));

    auto rv1 = tt::CallLuaFunction(lua, "onInit", "scene1", {{ LUA_REGISTRYINDEX, s1idx}});
    BOOST_TEST(rv1.has_value());
    BOOST_TEST(rv1->size() == 1);
    BOOST_TEST(tt::GetLuaValue<std::string>(rv1->front()) == "mylua1");

    auto rv2 = tt::CallLuaFunction(lua, "onInit", "scene2", { LUA_REGISTRYINDEX, s2idx});
    BOOST_TEST(rv2.has_value());
    BOOST_TEST(rv2->size() == 1);
    BOOST_TEST(tt::GetLuaValue<std::string>(rv2->front()) == "mylua2");
}

constexpr auto main_file = R"lua(
require "libs/includefile.lua"
function mainFunction()
return includeFunction()
end
)lua";

constexpr auto include_file = R"lua(
function includeFunction()
return "test1234"
end
)lua";

BOOST_AUTO_TEST_CASE(luaIncludeTest)
{
    GameScreenStub stub;
    lua_State* lua = luaL_newstate();

    auto path = tt::tempFolder();
    tt::ResourceManager resources { path / "resources", nullptr };

    tt::initLua(lua, stub, nullptr, &resources);
    BOOST_TEST_REQUIRE(lua != nullptr);

    const auto mainfile = (path / "resources" / "lua" / "scene1.lua").string();
    const auto incfile = (path / "resources" / "lua" / "libs" / "includefile.lua").string();

    writeFile(mainfile, main_file);
    writeFile(incfile, include_file);

    SceneStub s1{ "scene1" };
    auto s1idx = tt::registerObject(lua, s1);

    BOOST_TEST(tt::loadSceneLuaFile(s1, mainfile, lua));

    auto rv1 = tt::CallLuaFunction(lua, "mainFunction", "scene1", {{ LUA_REGISTRYINDEX, s1idx}});
    BOOST_REQUIRE(rv1.has_value());
    BOOST_REQUIRE(rv1->size() == 1);
    BOOST_TEST(tt::GetLuaValue<std::string>(rv1->front()) == "test1234");
}

constexpr auto playerTestFile = R"lua(
function doHealth(scene)
    local player = scene:getPlayer()
    local health = player:getHealth()
    player:setHealth(health - 25)
end
function doBalance(scene)
    local player = scene:getPlayer()
    local b = player:getBalance()
    player:setBalance(b - 2.50)
end
)lua";

BOOST_AUTO_TEST_CASE(luaPlayerTest)
{
    const auto path{ tt::tempFolder() };
    const auto luapath{ path / "resources" / "lua" };
    const auto mappath{ path / "resources" / "maps" };

    writeFile((luapath / "scene1.lua").string(), playerTestFile);
    writeFile((mappath / "scene1.json").string(), 
        R"({"background":{"tiles":{ "x": 16, "y": 16 }}, "player": { "state": "up" }})");

    TestHarness harness{ (path / "resources").string() };
    auto lua = harness._lua;
    auto player = harness._player;

    auto scene = std::make_shared<tt::Scene>("scene1", harness.setup());
    scene->enter();

    BOOST_TEST(player->health() == 100);
    tt::CallLuaFunction(lua, "doHealth", "scene1", {{ LUA_REGISTRYINDEX, scene->luaIdx() }});
    BOOST_TEST(player->health() == 75);

    BOOST_TEST(player->balance() == 40.00, boost::test_tools::tolerance(0.001));
    tt::CallLuaFunction(lua, "doBalance", "scene1", { { LUA_REGISTRYINDEX, scene->luaIdx() } });
    BOOST_TEST(player->balance() == 37.50, boost::test_tools::tolerance(0.001));
}

BOOST_AUTO_TEST_CASE(playerPositionTest)
{
    const auto path{ tt::tempFolder() };
    const auto luapath{ path / "resources" / "lua" };
    const auto mappath{ path / "resources" / "maps" };

    writeFile((mappath / "scene1.json").string(), 
        R"({"onEnter":"onEnter",
            "background":{"tiles":{ "x": 16, "y": 16 }},
            "player":{"state":"up","start": { "x": 2, "y": 2 }}})");

    writeFile((luapath / "scene1.lua").string(),
        R"(s = nil
p=nil

function onEnter(scene)
    s = scene
    p = scene:getPlayer()
end

function tile_setter(x,y)
    s:setPlayerTile(x,y)
end

function tile_getter()
    local x,y = s:getPlayerTile()
    return x,y
end

function global_setter(x,y)
    p:setPosition(x,y)
end

function global_getter()
    local x,y = p:getPosition()
    return x,y
end
)");

    TestHarness harness{ (path / "resources").string() };
    auto L = harness._lua;
    auto player = harness._player;

    auto scene = std::make_shared<tt::Scene>("scene1", harness.setup());
    scene->init();
    scene->enter();

    // sanity check
    BOOST_TEST((scene->getPlayerTile() == sf::Vector2f{ 2.f, 2.f }));
    
    auto results = tt::CallLuaFunction(L, "global_getter", "scene1");
    BOOST_TEST(results.has_value());
    BOOST_TEST(results->size() == 2);
    BOOST_TEST(tt::GetLuaValue<float>(results->at(0)) == 32.f, boost::test_tools::tolerance(0.001));
    BOOST_TEST(tt::GetLuaValue<float>(results->at(1)) == 32.f, boost::test_tools::tolerance(0.001));
    results->clear();

    results = tt::CallLuaFunction(L, "tile_getter", "scene1");
    BOOST_TEST(results.has_value());
    BOOST_TEST(results->size() == 2);
    BOOST_TEST(tt::GetLuaValue<float>(results->at(0)) == 2.f, boost::test_tools::tolerance(0.001));
    BOOST_TEST(tt::GetLuaValue<float>(results->at(1)) == 2.f, boost::test_tools::tolerance(0.001));
    results->clear();

    tt::CallLuaFunction(L, "global_setter", "scene1",
        {
            MakeLuaArg(50.f), 
            MakeLuaArg(50.f)
        });

    results = tt::CallLuaFunction(L, "tile_getter", "scene1");
    BOOST_TEST(results.has_value());
    BOOST_TEST(results->size() == 2);
    BOOST_TEST(tt::GetLuaValue<float>(results->at(0)) == 3.f, boost::test_tools::tolerance(0.001));
    BOOST_TEST(tt::GetLuaValue<float>(results->at(1)) == 3.f, boost::test_tools::tolerance(0.001));
    results->clear();

    tt::CallLuaFunction(L, "tile_setter", "scene1",
        {
            MakeLuaArg(1.f),
            MakeLuaArg(1.f)
        });

    results = tt::CallLuaFunction(L, "global_getter", "scene1");
    BOOST_TEST(results.has_value());
    BOOST_TEST(results->size() == 2);
    BOOST_TEST(tt::GetLuaValue<float>(results->at(0)) == 16.f, boost::test_tools::tolerance(0.001));
    BOOST_TEST(tt::GetLuaValue<float>(results->at(1)) == 16.f, boost::test_tools::tolerance(0.001));
}

const std::tuple<std::string, std::string> itemTestData[] = 
{
    { "bag-of-weed", "Bag of Weed" },
    { "weed-pipe", "Weed Pipe" },
    { "magnifying-glass", "Magnifying Glass" },
    { "bag-of-crack", "Bag of Crack"}
};

// --run_test=tt/lua/luaItemCreateTest
BOOST_DATA_TEST_CASE(luaItemCreateTest, data::make(itemTestData), id, expected)
{
    const auto testscript = fmt::format(R"lua(
local item = ItemFactory.createItem('{}')
return item:name()
)lua", id);

    const auto resfolder = fmt::format("{}/resources", TT_SRC_DIRECTORY_);
    TestHarness harness{ resfolder };
    auto L = harness._lua;

    // load the test script
    luaL_dostring(L, testscript.c_str());
    const auto retval = lua_tostring(L, -1);
    BOOST_TEST(retval == expected);
}

const auto luaItemPlayerTestLua = R"lua(
newitem = nil
player = nil
function onEnter_addItem(scene)
    player = scene:getPlayer()
    newitem = ItemFactory.createItem("key")
    player:addItem(newitem)
end

function removeKey()
    player:removeItem(newitem)
end
)lua";

const auto luaItemPlayerTestJSON = R"lua(
{
    "onEnter": "onEnter_addItem",
    "background": { "tiles": { "x": 16, "y": 16 } },
    "player": { "state": "up" }
}
)lua";

// --run_test=tt/lua/itemPlayerTest
BOOST_AUTO_TEST_CASE(itemPlayerTest)
{
    const auto path{ tt::tempFolder() };
    const auto luapath{ path / "resources" / "lua" };
    const auto mappath{ path / "resources" / "maps" };
    const auto itemspath{ path / "resources" / "items" };

    writeFile((mappath / "scene1.json").string(), luaItemPlayerTestJSON);
    writeFile((luapath / "scene1.lua").string(), luaItemPlayerTestLua);

    // copy the source's items folder to the test folder
    const auto itemsrc { fmt::format("{}/resources/items", TT_SRC_DIRECTORY_) };
    const auto itemdst { (path / "resources" / "items").string() };
    tt::copyDirectory(itemsrc, itemdst);

    TestHarness harness{ (path / "resources").string() };
    auto player = harness._player;
    auto L = harness._lua;

    auto scene = std::make_shared<tt::Scene>("scene1", harness.setup());

    BOOST_TEST(!player->hasItem("key"));
    scene->enter();
    BOOST_TEST(player->hasItem("key"));

    tt::CallLuaFunction(L, "removeKey", "scene1", { { LUA_REGISTRYINDEX, scene->luaIdx() } });

    BOOST_TEST(!player->hasItem("key")); 
}

const auto luaItemSceneTestLua = R"lua(
function onEnter_addItem(scene)
    local newitem = ItemFactory.createItem("key")
    scene:addItem(newitem)
end

function removeThing(scene, item)
    scene:removeItem(item)
end
)lua";

const auto luaItemSceneTestJSON = R"lua(
{
    "onEnter" : "onEnter_addItem",
    "player": { "state": "up" },
    "background":
    {
        "tiles": { "x": 16, "y": 16 }
    },
    "items":
    {
        "sax":
        {
            "instances":
            [    
                { "x": 5, "y": 5 }
            ]
        }
    }
}
)lua";

// --run_test=tt/lua/itemSceneTest
BOOST_AUTO_TEST_CASE(itemSceneTest)
{ 
    const auto path{ tt::tempFolder() };
    const auto luapath{ path / "resources" / "lua" };
    const auto mappath{ path / "resources" / "maps" };
    const auto itemspath{ path / "resources" / "items" };

    writeFile((mappath / "scene1.json").string(), luaItemSceneTestJSON);
    writeFile((luapath / "scene1.lua").string(), luaItemSceneTestLua);

    // copy the source's items folder to the test folder
    const auto itemsrc{ fmt::format("{}/resources/items", TT_SRC_DIRECTORY_) };
    const auto itemdst{ (path / "resources" / "items").string() };
    tt::copyDirectory(itemsrc, itemdst);

    TestHarness harness{ (path / "resources").string() };

    auto scene = std::make_shared<tt::Scene>("scene1", harness.setup());
    scene->init();
    BOOST_TEST(scene->items().size() == 1);
    scene->enter();
    BOOST_TEST(scene->items().size() == 2);

    auto firstItem = scene->items().front();
    tt::CallLuaFunction(harness._lua, "removeThing", "scene1",
        { 
            { LUA_REGISTRYINDEX, scene->luaIdx() },
            { LUA_TLIGHTUSERDATA, static_cast<void*>(&firstItem) } 
        });
    BOOST_TEST(scene->items().size() == 1);
    BOOST_TEST(scene->items().front()->getID() == "key");
}

// --run_test=tt/lua/itemPickUpTest
BOOST_AUTO_TEST_CASE(itemPickUpTest)
{
    const auto path{ tt::tempFolder() };
    const auto luapath{ path / "resources" / "lua" };
    const auto mappath{ path / "resources" / "maps" };
    const auto itemspath{ path / "resources" / "items" };

    writeFile((mappath / "scene1.json").string(), R"lua(
{
    "background": { "tiles": { "x": 16, "y": 16 } },
    "player": { "state": "up" },
    "items":
    {
        "sax":
        {
            "instances":
            [    
                { "x": 5, "y": 5, "onSelect": "sax_onSelect" }
            ]
        }
    }
}
)lua");

    writeFile((luapath / "scene1.lua").string(), R"lua(
function sax_onSelect(scene, item)
    local player = scene:getPlayer()
    player:addItem(item)
    scene:removeItem(item)
end
)lua");

    // copy the source's items folder to the test folder
    const auto itemsrc { fmt::format("{}/resources/items", TT_SRC_DIRECTORY_) };
    const auto itemdst { (path / "resources" / "items").string() };
    tt::copyDirectory(itemsrc, itemdst);

    TestHarness harness{ (path / "resources").string() };
    auto player = harness._player;
    auto L = harness._lua;

    auto scene = std::make_shared<tt::Scene>("scene1", harness.setup());
    scene->init();
    scene->enter();

    BOOST_TEST(scene->items().size() == 1);
    BOOST_TEST(scene->items().front()->getID() == "sax");
    BOOST_TEST(player->getInventory().size() == 0);

    auto sax = scene->items().front();
    void* saxPtr = static_cast<void*>(sax.get());

    tt::CallLuaFunction(L, "sax_onSelect", "scene1",
        { 
            { LUA_REGISTRYINDEX, scene->luaIdx() },
            { LUA_TLIGHTUSERDATA, static_cast<void*>(&sax) } 
        });

    BOOST_TEST(scene->items().size() == 0);
    BOOST_TEST(player->getInventory().size() == 1);

    const auto& newsax = player->getInventory().front();
    BOOST_TEST(newsax->getID() == "sax");

    // make sure it's the same underlying object
    BOOST_TEST(static_cast<void*>(newsax.get()) == saxPtr); 
}

// --run_test=tt/lua/itemPropertyTest
BOOST_AUTO_TEST_CASE(itemPropertyTest)
{
    const auto path{ tt::tempFolder() };
    const auto luapath{ path / "resources" / "lua" };
    const auto mappath{ path / "resources" / "maps" };
    const auto itemspath{ path / "resources" / "items" };

    // copy the bag PNG to the test folder
    const auto itemsrc{ fmt::format("{}/resources/items", TT_SRC_DIRECTORY_) };
    copyFile((fs::path{ itemsrc } / "bag.png"), itemspath / "bag.png");

    writeFile((itemspath / "bag.json").string(),
        R"x({
    "name":         "Bag of Crack",
    "description":  "This is good for your blood pressure.",
    "obtainable":   false
})x");

    writeFile((mappath / "scene1.json").string(),
        R"({
    "background":{"tiles":{ "x": 16, "y": 16 }},
    "player":{"state":"up","start": { "x": 35, "y": 35 }},
    "items": {"bag": { "instances": [ { "x": 5, "y": 5 } ] }}
})");

    writeFile((luapath / "scene1.lua").string(),
        R"(
function getId(item)
    local id = item:id()
    return id
end

function getName(item)
    local name = item:name()
    return name
end

)");

    TestHarness harness{ (path / "resources").string() };
    auto scene = std::make_shared<tt::Scene>("scene1", harness.setup());
    scene->init();
    scene->enter();

    auto& items = scene->items();
    const auto bag = std::find_if(items.begin(), items.end(), [](auto item) { return item->getID() == "bag";  });

    BOOST_TEST((bag != items.end()));

    auto results = tt::CallLuaFunction(harness._lua, "getId", "scene1",
        {
            { LUA_REGISTRYINDEX, (*bag)->luaIdx() } // `bag` is an iterator to a shared_ptr
        });

    BOOST_TEST(results.has_value());
    BOOST_TEST(results->size() == 1);
    BOOST_TEST(tt::GetLuaValue<std::string>(results->at(0)) == "bag");

    results = tt::CallLuaFunction(harness._lua, "getName", "scene1",
        {
            { LUA_REGISTRYINDEX, (*bag)->luaIdx() } // `bag` is an iterator to a shared_ptr
        });

    BOOST_TEST(results.has_value());
    BOOST_TEST(results->size() == 1);
    BOOST_TEST(tt::GetLuaValue<std::string>(results->at(0)) == "Bag of Crack");
}

BOOST_AUTO_TEST_SUITE_END() // lua
BOOST_AUTO_TEST_SUITE_END() // tt
