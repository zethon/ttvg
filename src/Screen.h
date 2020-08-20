#pragma once

#include <memory>

#include <boost/any.hpp>

#include <SFML/Graphics.hpp>

#include "ResourceManager.h"
#include "IUpdateable.h"

namespace tt
{

constexpr std::uint16_t SCREEN_SPLASH = 10;
constexpr std::uint16_t SCREEN_INTRO = 20;
constexpr std::uint16_t SCREEN_SHART = 30;
constexpr std::uint16_t SCREEN_GAME = 40;
constexpr std::uint16_t SCREEN_GAMEOVER = 50;

using DrawablePtr = std::shared_ptr<sf::Drawable>;

enum class ScreenActionType
{
    NONE = 0,
    CHANGE_SCREEN,
    CHANGE_SCENE
};

struct ScreenAction
{
    ScreenActionType    type;
    boost::any            data;
};

class Screen
{

public:
    Screen(ResourceManager& res, sf::RenderTarget& target);

    virtual ~Screen() = default;

    void addDrawable(DrawablePtr drawable);
    void clearDrawable();
    const std::vector<DrawablePtr>& getDrawables() const
    {
        return _objects;
    }

    void addUpdateable(IUpdateablePtr updateable);
    void removeUpdateable(IUpdateablePtr updateable);
    void clearUpdateable();

    // iterate all draw'able obects
    virtual void draw();

    // poll system/user events
    [[maybe_unused]] virtual ScreenAction poll(const sf::Event&);

    // update positions and state
    [[maybe_unused]] virtual ScreenAction timestep();

    // clean up any resources
    virtual void close()
    {
        clearDrawable();
        clearUpdateable();
    }

    sf::FloatRect getObservableRect() const
    {
        const auto view = _window.getView();
        const auto size = view.getSize();
        const auto center = view.getCenter();
        auto x = center.x - (size.x / 2);
        auto y = center.y - (size.y / 2);
        return { x, y, size.x, size.y };
    }

    void setVisible(bool var) { _visible = var; }
    bool visible() const { return _visible; }

protected:
    std::vector<DrawablePtr>        _objects;
    std::vector<IUpdateablePtr>     _updateables;

    ResourceManager&                _resources;
    sf::RenderTarget&               _window;

    bool                            _visible = true;
};

} // namespace tt