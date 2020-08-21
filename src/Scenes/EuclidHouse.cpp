#include <fmt/core.h>

#include "EuclidHouse.h"

namespace nl = nlohmann;

namespace tt
{
    
constexpr auto SCENE_NAME = "EuclidHouse";

EuclidHouse::EuclidHouse(ResourceManager& resmgr, sf::RenderTarget& target, PlayerPtr player)
    : Scene(SCENE_NAME, resmgr, target, player),
      _hud{ resmgr, target },
      _descriptionText{ resmgr, target }
{
}

PollResult EuclidHouse::poll(const sf::Event& e)
{
    auto retval = Scene::poll(e);
    if (retval.handled) return retval;

    if (e.type == sf::Event::KeyPressed)
    {
        switch (e.key.code)
        {
            default:
            break;

            case sf::Keyboard::Period:
            {
                _hud.setVisible(!_hud.visible());
            }
            break;
        }
    }

    return retval;
}

void EuclidHouse::draw()
{
    Scene::draw();
    _window.draw(*_player);

    _window.setView(_window.getDefaultView());
    _hud.draw();
    _descriptionText.draw();
}

void EuclidHouse::updateCurrentTile(const TileInfo & info)
{
    _currentTile = info;

    switch (_currentTile.type)
    {
        default:
            _hud.setZoneText({});
            _descriptionText.setText({});
        break;

        case TileType::ZONE:
        {
            const auto zoneinfo = boost::any_cast<Zone>(_currentTile.data);
            _hud.setZoneText(zoneinfo.name);
            if (!zoneinfo.description.empty())
            {
                _descriptionText.setText(zoneinfo.description);
            }
        }
        break;
    }
}

} // namespace tt
