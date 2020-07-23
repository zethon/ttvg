#include <stdio.h>
#include <cassert>
#include <optional>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/spirit/home/x3.hpp>

#include <fmt/core.h>

#include <SFML/Graphics.hpp>

#include "ResourceManager.h"
#include "TTUtils.h"
#include "Screen.h"
#include "Engine.h"

namespace po = boost::program_options;
namespace x3 = boost::spirit::x3;

constexpr char window_title[] = "Lord of the Dumpsters: A Tommy Tooter Game";
constexpr std::size_t window_width = 1280;
constexpr std::size_t window_height = 720;

int main(int argc, char *argv[])
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,?", "print help message")
        ("version,v", "print version string")
        ("resources,r", po::value<std::string>(), "path of resource folder")
        ("screen,s", po::value<std::uint16_t>(), "start screen id")
        ("window-size,w", po::value<std::string>(), "window size")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    std::string resourceFolder = tt::defaultResourceFolder();
    if (vm.count("resources") > 0)
    {
        resourceFolder = vm["resources"].as<std::string>();
    }

    std::size_t width   = window_width;
    std::size_t height  = window_height;
    if (vm.count("window-size") > 0)
    {
        std::string windowsize = vm["window-size"].as<std::string>();

        auto w = [&](auto& ctx) { width = _attr(ctx); };
        auto h = [&](auto& ctx) { height = _attr(ctx); };
        bool r = x3::phrase_parse(windowsize.begin(), windowsize.end(),
            (
                x3::int_[w] >> 'x' >> x3::int_[h]
            ),
            x3::space);

        if (!r)
        {
            std::cerr << fmt::format("invalid screen size '{}'\n", windowsize);
            return 1;
        }
    }

    auto win = std::make_shared<sf::RenderWindow>( 
        sf::VideoMode(  static_cast<unsigned int>(width), 
                        static_cast<unsigned int>(height) ),
        window_title, 
        sf::Style::Titlebar | sf::Style::Close
    );

    win->setFramerateLimit(60);

    // leading spaces can cause problems on macOS
    boost::algorithm::trim(resourceFolder);

    tt::TooterEngine engine{ boost::filesystem::path{resourceFolder}, win };

    if (vm.count("screen") > 0)
    {
        engine.changeScreen(vm["screen"].as<std::uint16_t>());
    }

    while (win->isOpen()) 
    {
        sf::Event e;
        while (win->pollEvent(e)) 
        {
            switch (e.type) 
            {
                default:
                    engine.poll(e);
                break;

                case sf::Event::EventType::Closed:
                    win->close();
                break;
            }
        }

        engine.timestep();

        win->clear();
        engine.drawScreen();
        win->display();
    }
    return 0;
}
