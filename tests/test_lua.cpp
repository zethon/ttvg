#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <fmt/format.h>

#include <SFML/Graphics.hpp>

#include "../src/ResourceManager.h"
#include "../src/GameScreen.h"
#include "../src/TTLua.h"
#include "../src/Scenes/Scene.h"

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
        return tt::SceneSetup {
            _resources,
            _window,
            _player,
            _lua,
            _itemFactory 
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

// std::string callFunction(const std::string& funcname, lua_State* L, const std::string& envname, int objIdx)
// {
//     // first get the execution environment and set that
//     lua_getglobal(L, envname.c_str()); // 1:env
//     BOOST_TEST((lua_isnil(L, 1) == 0));

//     // now load up the init function
//     lua_getfield(L, 1, funcname.c_str()); // 1:env, 2:func
//     BOOST_TEST((lua_isnil(L, 2) == 0));
//     BOOST_TEST((lua_isfunction(L, 2) == 1));

//     // now get the parameter we're passing to Lua which is a Scene* (aka `this`)
//     lua_rawgeti(L, LUA_REGISTRYINDEX, objIdx); // 1:env, 2:func, 1:ud
//     if (lua_pcall(L, 1, 1, 0) != 0) // 1:env, 2:retval
//     {
//         auto error = lua_tostring(L, -1);
//         BOOST_TEST(false, error);
//     }

//     std::string retval = lua_tostring(L, -1);
//     lua_settop(L, 0);
//     return retval;
// }

// std::string callGlobalFunction(const std::string& funcname, lua_State* L, const std::string& envname, int objIdx)
// {
//     // first get the execution environment and set that
//     lua_getglobal(L, envname.c_str()); // 1:env
//     BOOST_TEST((lua_isnil(L, 1) == 0));

//     // now load up the init function
//     lua_getfield(L, 1, funcname.c_str()); // 1:env, 2:func
//     BOOST_TEST((lua_isnil(L, 2) == 0));
//     BOOST_TEST((lua_isfunction(L, 2) == 1));

//     // now get the parameter we're passing to Lua which is a Scene* (aka `this`)
//     lua_rawgeti(L, LUA_REGISTRYINDEX, objIdx); // 1:env, 2:func, 1:ud
//     if (lua_pcall(L, 1, 1, 0) != 0) // 1:env, 2:retval
//     {
//         auto error = lua_tostring(L, -1);
//         BOOST_TEST(false, error);
//     }

//     std::string retval = lua_tostring(L, -1);
//     lua_settop(L, 0);
//     return retval;
// }

BOOST_AUTO_TEST_CASE(loadTestCase)
{
    GameScreenStub stub;
    lua_State* lua = luaL_newstate();

    tt::initLua(lua, stub, nullptr);
    BOOST_TEST_REQUIRE(lua != nullptr);

    auto path = tt::tempFolder();
    const auto luafile1 = (path / "file1.lua").string();
    const auto luafile2 = (path / "file2.lua").string();

    writeFile(luafile1, file1);
    writeFile(luafile2, file2);

    SceneStub s1{ "scene1" };
    auto s1idx = tt::registerScene(lua, s1);

    SceneStub s2{ "scene2" };
    auto s2idx = tt::registerScene(lua, s2);

    BOOST_TEST(tt::loadSceneLuaFile(s1, luafile1, lua));
    BOOST_TEST(tt::loadSceneLuaFile(s2, luafile2, lua));

    auto rv1 = tt::CallLuaFunction(lua, "onInit", "scene1", {{ LUA_REGISTRYINDEX, s1idx}});
    BOOST_TEST(rv1.size() == 1);
    BOOST_TEST(tt::GetLuaValue<std::string>(rv1.front()) == "mylua1");

    auto rv2 = tt::CallLuaFunction(lua, "onInit", "scene2", { LUA_REGISTRYINDEX, s2idx});
    BOOST_TEST(rv2.size() == 1);
    BOOST_TEST(tt::GetLuaValue<std::string>(rv2.front()) == "mylua2");
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
    writeFile((mappath / "scene1.json").string(), R"({"background":{"tiles":{ "x": 16, "y": 16 }}})");

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
    "background":
    {
        "tiles": { "x": 16, "y": 16 }
    }
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
    "background":
    {
        "tiles": { "x": 16, "y": 16 }
    },
    "items":
    {
        "sax": 
        [    
            { "x": 5, "y": 5 }
        ]
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

BOOST_AUTO_TEST_CASE(itemPickUpTest)
{
    const auto path{ tt::tempFolder() };
    const auto luapath{ path / "resources" / "lua" };
    const auto mappath{ path / "resources" / "maps" };
    const auto itemspath{ path / "resources" / "items" };

    writeFile((mappath / "scene1.json").string(), R"lua(
{
    "background": { "tiles": { "x": 16, "y": 16 } },
    "items":
    {
        "sax": 
        [    
            { "x": 5, "y": 5, "onPickUp": "sax_onPickup" },
        ]
    }
}
)lua");

    writeFile((luapath / "scene1.lua").string(), R"lua(
function sax_onPickup(scene, item)
end
)lua");

    // copy the source's items folder to the test folder
    const auto itemsrc{ fmt::format("{}/resources/items", TT_SRC_DIRECTORY_) };
    const auto itemdst{ (path / "resources" / "items").string() };
    tt::copyDirectory(itemsrc, itemdst);

}

BOOST_AUTO_TEST_SUITE_END() // lua
BOOST_AUTO_TEST_SUITE_END() // tt