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

class GameScreenStub{};

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

std::string callFunction(const std::string& funcname, lua_State* L, const std::string& envname, int objIdx)
{
    // first get the execution environment and set that
    lua_getglobal(L, envname.c_str()); // 1:env
    BOOST_TEST((lua_isnil(L, 1) == 0));

    // now load up the init function
    lua_getfield(L, 1, funcname.c_str()); // 1:env, 2:func
    BOOST_TEST((lua_isnil(L, 2) == 0));
    BOOST_TEST((lua_isfunction(L, 2) == 1));

    // now get the parameter we're passing to Lua which is a Scene* (aka `this`)
    lua_rawgeti(L, LUA_REGISTRYINDEX, objIdx); // 1:env, 2:func, 1:ud
    if (lua_pcall(L, 1, 1, 0) != 0) // 1:env, 2:retval
    {
        auto error = lua_tostring(L, -1);
        BOOST_TEST(false, error);
    }

    std::string retval = lua_tostring(L, -1);
    lua_settop(L, 0);
    return retval;
}

std::string callGlobalFunction(const std::string& funcname, lua_State* L, const std::string& envname, int objIdx)
{
    // first get the execution environment and set that
    lua_getglobal(L, envname.c_str()); // 1:env
    BOOST_TEST((lua_isnil(L, 1) == 0));

    // now load up the init function
    lua_getfield(L, 1, funcname.c_str()); // 1:env, 2:func
    BOOST_TEST((lua_isnil(L, 2) == 0));
    BOOST_TEST((lua_isfunction(L, 2) == 1));

    // now get the parameter we're passing to Lua which is a Scene* (aka `this`)
    lua_rawgeti(L, LUA_REGISTRYINDEX, objIdx); // 1:env, 2:func, 1:ud
    if (lua_pcall(L, 1, 1, 0) != 0) // 1:env, 2:retval
    {
        auto error = lua_tostring(L, -1);
        BOOST_TEST(false, error);
    }

    std::string retval = lua_tostring(L, -1);
    lua_settop(L, 0);
    return retval;
}

BOOST_AUTO_TEST_CASE(loadTestCase)
{
    GameScreenStub stub;
    lua_State* lua = luaL_newstate();

    tt::initLua(lua, stub);
    BOOST_TEST_REQUIRE(lua != nullptr);

    auto path = tt::tempFolder();
    const auto luafile1 = (path / "file1.lua").string();
    const auto luafile2 = (path / "file2.lua").string();

    writeFile(luafile1, file1);
    writeFile(luafile2, file2);

    SceneStub s1{ "scene1" };
    SceneStub s2{ "scene2" };

    auto s1idx = tt::loadSceneLuaFile(s1, luafile1, lua);
    auto s2idx = tt::loadSceneLuaFile(s2, luafile2, lua);

    BOOST_TEST(callFunction("onInit", lua, "scene1", s1idx) == "mylua1");
    BOOST_TEST(callFunction("onInit", lua, "scene2", s2idx) == "mylua2");
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

    tt::ResourceManager res{ path / "resources" };
    tt::NullWindow window;

    sf::Texture texture;
    texture.create(100, 100);
    auto player = std::make_shared<tt::Player>(texture, sf::Vector2i{ 10,10 });

    lua_State* lua = luaL_newstate();

    GameScreenStub stub;
    tt::initLua(lua, stub);

    tt::SceneSetup setup{ res, window, player, lua, nullptr };
    auto scene = std::make_shared<tt::Scene>("scene1", setup);
    scene->enter();

    BOOST_TEST(player->health() == 100);
    tt::CallLuaFunction(lua, "doHealth", "scene1", scene->luaIdx());
    BOOST_TEST(player->health() == 75);

    BOOST_TEST(player->balance() == 40.00, boost::test_tools::tolerance(0.001));
    tt::CallLuaFunction(lua, "doBalance", "scene1", scene->luaIdx());
    BOOST_TEST(player->balance() == 37.50, boost::test_tools::tolerance(0.001));
}

const std::tuple<std::string, std::string> itemTestData[] = 
{
    { "bag-of-weed", "Bag of Weed" },
    { "weed-pipe", "Weed Pipe" },
    { "magnifying-glass", "Magnifying Glass" },
    { "bag-of-crack", "Bag of Crack"}
};

// --run_test=tt/lua/luaItemTest
BOOST_DATA_TEST_CASE(luaItemTest, data::make(itemTestData), id, expected)
{
    const auto testscript = fmt::format(R"lua(
function testItem(scene)
    local item = scene:createItem('{}')
    return item:name()
end
)lua", id);

    tt::NullWindow window;

    const auto resfolder = fmt::format("{}/resources", TT_SRC_DIRECTORY_);
    tt::ResourceManager resources{ boost::filesystem::path { resfolder } };

    std::shared_ptr<ItemFactory> itemFactory = std::make_shared<ItemFactory>(resources);

    lua_State* lua = luaL_newstate();

    GameScreenStub stub;
    tt::initLua(lua, stub);

    sf::Texture texture;
    texture.create(100, 100);
    auto player = std::make_shared<tt::Player>(texture, sf::Vector2i{ 10,10 });

    tt::SceneSetup setup{ resources, window, player, lua, itemFactory };
    auto scene = std::make_shared<tt::Scene>("scene1", setup);
    scene->enter();

    // load the test script
    luaL_dostring(lua, testscript.c_str());

    lua_getglobal(lua, "testItem"); // 1:func
    lua_rawgeti(lua, LUA_REGISTRYINDEX, scene->luaIdx());
    if (lua_pcall(lua, 1, 1, 0) != 0)
    {
        const auto error = lua_tostring(lua, -1);
        BOOST_TEST(false, error);
    }
    else
    {
        const auto retval = lua_tostring(lua, -1);
        BOOST_TEST(retval == expected);
    }
}


BOOST_AUTO_TEST_SUITE_END() // lua
BOOST_AUTO_TEST_SUITE_END() // tt