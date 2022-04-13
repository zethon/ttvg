#include <stdio.h>
#include <cassert>
#include <optional>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/spirit/home/x3.hpp>

#include <fmt/core.h>

#include <spdlog/spdlog.h>
#include <spdlog/common.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include <SFML/Graphics.hpp>

#include "Screens/Screen.h"

#include "ResourceManager.h"
#include "TTUtils.h"
#include "Engine.h"
#include "core.h"
#include "TooterLogger.h"
#include "Settings.h"

namespace po = boost::program_options;
namespace x3 = boost::spirit::x3;
namespace fs = boost::filesystem;

constexpr std::size_t window_width = 1280;
constexpr std::size_t window_height = 720;

bool validateResourceFolder(std::string_view folder)
{
    fs::path f{ folder.data() };
    f = f / "images" / "splash.jpg";
    return fs::exists(f);
}

std::string getDefaultConfigFile()
{
    fs::path folder = tt::getUserFolder();
    return fs::path{folder / ".ttvg_config"}.string();
}

amb::SettingsPtr registerSettings()
{
    auto retval = std::make_unique<amb::Settings>();

    constexpr auto musicVolume = 100u;
    constexpr auto sfxVolume = 100u;

    retval->registerUInt("audio.volume.music", 100,
        std::make_shared<amb::RangeValidator<std::uint64_t>>(0, 100));

    retval->registerUInt("audio.volume.sfx", 100,
        std::make_shared<amb::RangeValidator<std::uint64_t>>(0, 100));

    retval->registerBool("logs.music.enabled", false);
    retval->registerBool("logs.sfx.enabled", false);

    return retval;
}

amb::SettingsPtr initSettings(std::string_view filename)
{
    auto settings = registerSettings();
    boost::filesystem::path configFile{ filename.data() };
    if (boost::filesystem::exists(configFile))
    {
        settings->load(filename);
    }
    else
    {
        settings->save(filename);
    }

    return settings;
}

void initLogging(std::string_view logfile)
{
    // create the root logger
    spdlog::stdout_color_mt(tt::log::GLOBAL_LOGGER);

#ifdef RELEASE
    spdlog::set_level(spdlog::level::info);
#else
    spdlog::set_level(spdlog::level::trace);
#endif

    if (logfile.size() > 0)
    {
        auto logger = tt::log::rootLogger();
        auto rotating = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            logfile.data(), 1024 * 1024 * 5, 3);

        logger->sinks().push_back(rotating);
    }
}

int main(int argc, char *argv[])
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,?", "print help message")
        ("config,c",po::value<std::string>(), "config file")
        ("logfile,l", po::value<std::string>(), "path of logfile")
        ("loglevel", po::value<std::string>(), "trace,debug,info,warning,error,critical,off")
        ("resources,r", po::value<std::string>(), "path of resource folder")
        ("screen,s", po::value<std::uint16_t>(), "start screen id")
        ("window-size,w", po::value<std::string>(), "window size")
        ("version,v", "print version string")
        ;

    po::variables_map vm;

    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    }
    catch(const po::error& e)
    {
        std::cerr << e.what();
    }

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 0;
    }
    else if (vm.count("version"))
    {
        std::cout << fmt::format("{}\n", APP_TITLE);
        std::cout << fmt::format("build-date : {}\n", BUILDTIMESTAMP);
        std::cout << std::endl;
        return 0;
    }

    std::string logfile;
    if (vm.count("logfile"))
    {
        logfile = vm["logfile"].as<std::string>();

        // leading spaces can cause problems on macOS
        boost::algorithm::trim(logfile);
    }

    initLogging(logfile);
    auto logger = tt::log::rootLogger();
    if (vm.count("loglevel"))
    {
        const auto configLevel = spdlog::level::from_str(vm["loglevel"].as<std::string>());
        logger->set_level(configLevel);
    }
    logger->info("Starting {} v{}",APP_NAME_LONG, VERSION);
    logger->info("built on {} for {}", BUILDTIMESTAMP, tt::getOsString());

    std::string configFile = getDefaultConfigFile();
    if (vm.count("config") > 0)
    {
        configFile = vm["config"].as<std::string>();
    }
    auto settings = initSettings(configFile);

    std::string resourceFolder = tt::defaultResourceFolder();
    if (vm.count("resources") > 0)
    {
        resourceFolder = vm["resources"].as<std::string>();
        
        // leading spaces can cause problems on macOS
        boost::algorithm::trim(resourceFolder);
    }
    logger->debug("resource folder: {}", resourceFolder);

    if (!validateResourceFolder(resourceFolder))
    {
        logger->critical("invalid resource folder: {}", resourceFolder);
        return 1;
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

        if (!r || width < 480 || height < 320)
        {
            logger->critical("invalid screen size '{}'\n", windowsize);
            return 1;
        }
    }
    logger->debug("initializing window size {}x{}", width, height);

    auto win = std::make_shared<sf::RenderWindow>( 
        sf::VideoMode(  static_cast<unsigned int>(width), 
                        static_cast<unsigned int>(height) ),
        APP_NAME_LONG,
        sf::Style::Titlebar | sf::Style::Close
    );

    win->setFramerateLimit(60);

    tt::TooterEngine engine{ fs::path{resourceFolder}, settings, win };

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
                {
                    const auto result = engine.poll(e);
                    if (result.handled
                        && result.action.type == tt::ScreenActionType::EXIT_GAME)
                    {
                        win->close();
                    }
                }
                break;

                case sf::Event::EventType::Closed:
                {
                    win->close();
                }
                break;
            }
        }

        engine.update();

        win->clear();
        engine.drawScreen();
        win->display();
    }

    logger->info("game shut down");
    return 0;
}
