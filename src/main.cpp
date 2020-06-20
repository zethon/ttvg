#include <cassert>
#include <filesystem>
#include <optional>

#include <boost/program_options.hpp>

#include <SFML/Graphics.hpp>

#include "ResourceManager.h"
#include "TTUtils.h"
#include "Screen.h"
#include "Engine.h"

namespace po = boost::program_options;

constexpr char window_title[] = "Tommy Tooter: The Official Game";
constexpr std::size_t window_width = 1024;
constexpr std::size_t window_height = 640;

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

    auto win = std::make_shared<sf::RenderWindow>(
            sf::VideoMode(window_width, window_height),
            window_title,
            sf::Style::Titlebar | sf::Style::Close
        );

    tt::TooterEngine engine{ resourceFolder, win };

    while (win->isOpen()) 
    {
        engine.poll();
        engine.timestep();

        sf::Event e;
        while (win->pollEvent(e)) 
        {
            switch (e.type) 
            {
                case sf::Event::EventType::Closed:
                    win->close();
                break;
            }
        }

        win->clear();
        engine.drawScreen();
        win->display();
    }
    return 0;
}