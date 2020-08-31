#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <fmt/format.h>

#include <SFML/Graphics.hpp>

#include "../src/ResourceManager.h"
#include "../src/GameScreen.h"
#include "../src/Scenes/Scene.h"

#include "Test.h"

namespace tools = boost::test_tools;
namespace data = boost::unit_test::data;

BOOST_AUTO_TEST_SUITE(tt)
BOOST_AUTO_TEST_SUITE(lua)

//// --run_test=tt/lua/loadTest
//BOOST_AUTO_TEST_CASE(loadTest)
//{
//    tt::NullWindow window;
//    const auto resfolder = fmt::format("{}/resources", TT_SRC_DIRECTORY_);
//    tt::ResourceManager resources{ boost::filesystem::path { resfolder } };
//
//    tt::GameScreen screen{ resources, window };
//    auto lua = screen.lua();
//
//    BOOST_TEST(lua != nullptr);
//    for (const auto&[name, scene] : screen.scenes())
//    {
//
//    }
//}

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

BOOST_AUTO_TEST_SUITE_END() // lua
BOOST_AUTO_TEST_SUITE_END() // tt