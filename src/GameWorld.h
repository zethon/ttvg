#pragma once

#include <boost/signals2.hpp>

#include <SFML/System/Time.hpp>

#include "date/date.h"

#include "TTUtils.h"

namespace tt
{

class GameWorld;
using GameCalendarPtr = std::shared_ptr<GameWorld>;

// the player will never see the year. when we start a new game we will
// pick an entirely new date, some time in 1975 because why not
class GameWorld
{
public:
    using Clock = std::chrono::system_clock;

    GameWorld();

    std::string datestr() const;
    std::string timestr() const;

    void update(const sf::Time& currentTime);

public: // signals
    boost::signals2::signal<void(const std::string& datestr, const std::string& timestr)> onUpdate;

private:
    void pickRandomTime();
    void updateText();

    std::chrono::system_clock::time_point _now;
    sf::Time    _lastUpdate = sf::Time::Zero;

    std::string _datestr;
    std::string _timestr;
};


} // namespace