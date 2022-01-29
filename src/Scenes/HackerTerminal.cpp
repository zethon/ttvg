#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244)
#endif

#include <algorithm>
#include <boost/process.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <boost/algorithm/string/join.hpp>

#include <fmt/core.h>

#include "HackerTerminal.h"
#include "../TTUtils.h"


namespace tt
{

namespace
{

std::string getHackerText()
{
    namespace bp = boost::process;
    bp::ipstream pipe_stream;

    const auto command = fmt::format("ls {}", getUserFolder());
    bp::child ch(command, bp::std_out > pipe_stream);

    std::string line;
    std::vector<std::string> lines;

    while (ch.running() && std::getline(pipe_stream, line) && !line.empty())
    {
        lines.push_back(line);
    }

    return boost::algorithm::join(lines, "\n");
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



