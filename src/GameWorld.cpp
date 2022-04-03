#include "GameWorld.h"

constexpr auto CLOCK_DELAY = 5u;

namespace tt
{

GameWorld::GameWorld()
{
    pickRandomTime();
    updateText();
}

std::string GameWorld::datestr() const
{
    return _datestr;
}

std::string GameWorld::timestr() const
{
    return _timestr;
}

void GameWorld::updateText()
{
    std::stringstream dss;
    dss << date::format("%a. %b %e", date::floor<std::chrono::hours>(_now));
    _datestr = dss.str();

    std::stringstream tss;
    tss << date::format("%H:%M %p", date::floor<std::chrono::minutes>(_now));
    std::string temp = tss.str();
    if (temp.at(0) == '0') temp.erase(temp.begin());
    _timestr = temp;
}

void GameWorld::pickRandomTime()
{
    // pick a random day (in 1975, but we never tell the player what year) and
    // do a little math to make sure the starting time is at 8:00am

    // 157795200 = Jan 1, 1975 - 0800
    auto epochstart = 157795200 + (tt::RandomNumber(0, 364) * 86400);
    _now = std::chrono::system_clock::time_point{std::chrono::seconds{epochstart}};

    // small hack so we start at exactly 8:00am
    _now -= std::chrono::minutes{10};
}

void GameWorld::timestep(const sf::Time& currentTime)
{
    auto elapsedTime{ currentTime - _lastUpdate };
    if (elapsedTime.asSeconds() > CLOCK_DELAY)
    {
        _now += std::chrono::minutes{10};

        updateText();

        _lastUpdate = currentTime;
        onUpdate(_datestr, _timestr);
    }
}

} // namespace tt