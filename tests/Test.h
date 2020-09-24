#pragma once
#include <iostream>

#include <boost/test/data/test_case.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <test-config.h>

namespace std
{

template<typename T>
std::ostream& operator<<(std::ostream& out, const sf::Vector2<T> item)
{
    auto[x, y] = item;
    out << "{ x=" << x
        << " y=" << y
        << " }";

    return out;
}

std::ostream& operator<<(std::ostream& out, const sf::FloatRect& item)
{
    auto [left, top, width, height] = item;
    out << "{ left=" << left
        << " top=" << top
        << " width=" << width
        << " height=" << height
        << "}";

    return out;
}

} // namespace std

namespace tt
{

class NullWindow : public sf::RenderTarget
{
public:
    sf::Vector2u getSize() const override
    {
        return sf::Vector2u{4096,4096};
    }
};

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

void writeFile(const std::string& file, const std::string& data)
{
    boost::filesystem::path filepath{ file };

    if (!boost::filesystem::exists(filepath.parent_path()))
    {
        boost::filesystem::create_directories(filepath.parent_path());
    }

    std::ofstream out(file);

    if (out.is_open())
    {
        out << data;
        out.close();
    }
}

namespace fs = boost::filesystem;
void copyDirectory(const fs::path& sourceDir, const fs::path& destinationDir)
{
    if (!fs::exists(sourceDir) || !fs::is_directory(sourceDir))
    {
        throw std::runtime_error("Source directory " + sourceDir.string() + " does not exist or is not a directory");
    }
    if (fs::exists(destinationDir))
    {
        throw std::runtime_error("Destination directory " + destinationDir.string() + " already exists");
    }
    if (!fs::create_directories(destinationDir))
    {
        throw std::runtime_error("Cannot create destination directory " + destinationDir.string());
    }

    for (const auto& dirEnt : fs::recursive_directory_iterator{sourceDir})
    {
        const auto& path = dirEnt.path();
        auto relativePathStr = path.string();
        boost::replace_first(relativePathStr, sourceDir.string(), "");
        fs::copy(path, destinationDir / relativePathStr);
    }
}

void copyFile(const fs::path& srcFile, const fs::path& dstFile)
{
    if (!fs::exists(srcFile) || !fs::is_regular_file(srcFile))
    {
        throw std::runtime_error("Source file " + srcFile.string() + " does not exist or it not a regular file");
    }

    if (!fs::create_directories(dstFile.parent_path()))
    {
        throw std::runtime_error("Cannot create destination directory " + dstFile.string());
    }

    fs::copy(srcFile, dstFile);
}

boost::filesystem::path tempFolder()
{
    auto temp = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path("ttvg%%%%%%");
    temp /= std::to_string(boost::unit_test::framework::current_test_case().p_id);
    boost::filesystem::create_directories(temp);
    return temp;
}

}