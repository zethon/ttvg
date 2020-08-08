#include "Scene.h"

namespace tt
{

Scene::Scene(ResourceManager& res, sf::RenderTarget& target, PlayerPtr player)
    : Screen(res, target),
      _weakPlayer{ player }
{
}

void Scene::enter()
{
    assert(!_player);
    _player = _weakPlayer.lock();
    _player->setPosition(_lastPlayerPos);
}

void Scene::exit()
{
    assert(_player);
    _lastPlayerPos = _player->getPosition();
    _player.reset();
}

//void Scene::updateCurrentTile(const TileInfo & info)
//{
//}

void Scene::init()
{
    createItems();
}

bool Scene::walkPlayer(std::uint32_t stepsize)
{
    const auto stepSize = stepsize
        + (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) ? 20 : 0);

    bool moved = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        auto xx = _player->getGlobalLeft();
        auto[x, y] = _player->getPosition();
        assert(xx >= 0);
        if (xx == 0) return false;

        xx -= stepSize;
        if (xx < 0) xx = 0;

        _player->setGlobalLeft(xx);
        moved = true;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        const auto boundaryRight = _background->getGlobalBounds().width;

        auto x = _player->getGlobalRight();
        assert(x <= boundaryRight);
        if (x == boundaryRight) return false;

        x += stepSize;
        if (x > boundaryRight) x = boundaryRight;
        _player->setGlobalRight(x);
        moved = true;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        auto y = _player->getGlobalTop();
        assert(y >= 0);
        if (y == 0) return false;

        y -= stepSize;
        if (y < 0) y = 0;
        _player->setGlobalTop(y);
        moved = true;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        const auto boundaryBottom = _background->getGlobalBounds().height;

        auto y = _player->getGlobalBottom();
        if (y >= boundaryBottom) return false;

        y += stepSize;
        if (y > boundaryBottom)
        {
            y = boundaryBottom;
        }
        _player->setGlobalBottom(y);
        moved = true;
    }

    return moved;
}

void Scene::createItems()
{
    //
    // Create items
    //
    auto itemFactory = ItemFactory(_resources);

    const auto& config = _background->json();

    //
    // Check if items are present in the map .json
    //
    if (config.find("items") != config.end())
    {
        // std::cout << "DEBUG found map items." << std::endl;

        //
        // Iterate over all item names.
        //
        for (auto& el : config["items"].items())
        {
            const auto& itemId = el.key();

            //
            // The associated value is a list of objects representing
            // coordinate pairs.
            //
            const auto& list = el.value();

            //
            // For each coordinate pair, create an item of this type
            // at the specified location on the map and add it to the 
            // _items vector.
            //
            for (auto& coords : list.items())
            {
                const auto& c = coords.value();
                sf::Vector2f position = 
                    _background->getGlobalFromTile(sf::Vector2f(c["x"].get<float>(), c["y"].get<float>()));

                ItemPtr i = itemFactory.createItem(itemId, position);
                _items.push_back(i);
            }
        }
    }
}

} // namespace tt