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

// --run_test=tt/lua/loadTest
BOOST_AUTO_TEST_CASE(loadTest)
{
    tt::NullWindow window;
    const auto resfolder = fmt::format("{}/resources", TT_SRC_DIRECTORY_);
    tt::ResourceManager resources{ boost::filesystem::path { resfolder } };

    tt::GameScreen screen{ resources, window };
    auto lua = screen.lua();

    BOOST_TEST(lua != nullptr);
    for (const auto&[name, scene] : screen.scenes())
    {

    }
}

BOOST_AUTO_TEST_SUITE_END() // lua
BOOST_AUTO_TEST_SUITE_END() // tt