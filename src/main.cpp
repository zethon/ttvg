#include <cassert>
#include <filesystem>
#include <optional>

#include <boost/program_options.hpp>

#include <SFML/Graphics.hpp>

#include "TTUtils.h"

namespace po = boost::program_options;

constexpr char window_title[] = "Tommy Tooter: The Official Game";
constexpr std::size_t window_width = 1024;
constexpr std::size_t window_height = 640;

class ResourceManager
{
    std::filesystem::path _resourceFolder;

public:

    ResourceManager(const std::filesystem::path& path)
        : _resourceFolder{ path }
    {}

    sf::Font loadFont(const std::string& name)
    {
        auto filepath = _resourceFolder / name;
        sf::Font font;
        if (font.loadFromFile(filepath.string())) 
        {
            return font;
        }

        return {};
    }
};

class Screen
{
    using DrawablePtr = std::shared_ptr<sf::Drawable>;
    std::vector<DrawablePtr>   _objects;

public:
    explicit Screen(ResourceManager& res)
        : _resources{ res }
    {}

    void draw(sf::RenderWindow& window)
    {
        for (const auto& object : _objects)
        {
            window.draw(*object);
        }
    }

    void addDrawable(std::shared_ptr<sf::Text> drawable)
    {
        _objects.push_back(drawable);
    }

protected:
    ResourceManager&            _resources;
};

class IntroScreen : public Screen
{
    sf::Font    _font;

public:
    explicit IntroScreen(ResourceManager& mgr)
        : Screen(mgr)
    {
        _font = _resources.loadFont("hobo.ttf");
        auto text = std::make_shared<sf::Text>("Hello Hobo!", _font);
        addDrawable(text);
    }
};

class GameScreen : public Screen
{
public:
    using Screen::Screen;
};

class ExitScreen : public Screen
{
public:
    using Screen::Screen;
};

class TooterEngine
{
    ResourceManager             _resourceManager;
    std::shared_ptr<Screen>     _currentScreen;
    
public:
    TooterEngine(const std::filesystem::path& respath)
        : _resourceManager{ respath }
    {
        _currentScreen = std::make_shared<IntroScreen>(_resourceManager);
    }

    void drawScreen(sf::RenderWindow& window)
    {
        if (_currentScreen)
        {
            _currentScreen->draw(window);
        }
    }
};

//  scenes:
//  - intro screen
//  - main menu (play game, play clip?, exit)
//  - game play

int main(int argc, char *argv[])
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,?", "print help message")
        ("version,v", "print version string")
        ("resources,r", po::value<std::string>(), "path of resource folder")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    std::filesystem::path resourceFolder = tt::defaultResourceFolder();
    if (vm.count("resources") > 0)
    {
        resourceFolder = std::filesystem::path(vm["resources"].as<std::string>());
    }

    sf::RenderWindow sfmlWin(
        sf::VideoMode(window_width, window_height), 
        window_title,
        sf::Style::Titlebar | sf::Style::Close);

    TooterEngine engine{ resourceFolder };



    sf::Font font;
    //You need to pass the font file location
     if (!font.loadFromFile(
        R"(C:\Users\aclau\src\ttvg\resources\hobo.ttf)"
     )) {
         return -1;
     }
    //sf::Text message("Hello, World !", font);
     std::shared_ptr<sf::Text> message = std::make_shared<sf::Text>("Hello 123", font);

    while (sfmlWin.isOpen()) 
    {
        sf::Event e;
        while (sfmlWin.pollEvent(e)) 
        {
            switch (e.type) 
            {
                case sf::Event::EventType::Closed:
                    sfmlWin.close();
                break;
            }
        }

        sfmlWin.clear();
        //sfmlWin.draw(*message);
        engine.drawScreen(sfmlWin);
        sfmlWin.display();
    }
    return 0;
}