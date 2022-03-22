#include <iostream>

#include <fmt/core.h>

#include "Item.h"
#include "TTUtils.h"
#include "TTLua.h"

namespace nl = nlohmann;

namespace tt
{

void from_json(const nl::json& j, ItemInfo& i)
{
    if (j.contains("name")) j["name"].get_to(i.name);
    if (j.contains("description")) j["description"].get_to(i.description);

    if (j.contains("texture"))
    {
        j["texture"].get_to(i.texturefile);
    }
    
    if (j.contains("obtainable"))
    {
        i.obtainable = j["obtainable"].get<bool>();
    }

    if (j.contains("size"))
    {
        i.size = j["size"].get<sf::Vector2u>();
    }

    if (j.contains("scale"))
    {
        i.scale = j["scale"].get<sf::Vector2f>();
    }

    if (j.contains("frame-count"))
    {
        i.framecount = j["frame-count"].get<std::uint32_t>();
    }

    if (j.contains("default-state"))
    {
        i.defaultState = j["default-state"].get<std::string>();
    }

    if (j.contains("time-step"))
    {
        i.timestep = j["time-step"].get<std::uint32_t>();
    }

    if (j.contains("respawn-delay"))
    {
        i.respawn = j["respawn-delay"].get<float>();
    }

    if (j.contains("hitbox"))
    {
        i.hitbox = j["hitbox"].get<tt::HitBox>();
    }

    if (j.contains("states") && j["states"].is_array())
    {
        for (const auto& item : j["states"].items())
        {
            auto state = item.value().get<ItemState>();
            if (!state.framecount.has_value())
            {
                state.framecount = i.framecount;
            }

            if (!state.timestep.has_value())
            {
                state.timestep = i.timestep;
            }

            if (!state.hitbox.has_value() && i.hitbox.has_value())
            {
                state.hitbox = i.hitbox;
            }

            i.states.insert(std::make_pair(state.id, std::move(state)));
        }
    }
}

void from_json(const nl::json& j, ItemState& state)
{
    if (j.contains("id"))
    {
        j["id"].get_to(state.id);
    }
    else
    {
        throw std::runtime_error("object state is missing 'id'");
    }

    if (j.contains("source"))
    {
        state.source = j["source"].get<sf::Vector2i>();
    }
    else
    {
        throw std::runtime_error("object state is missing 'source'");
    }

    if (j.contains("frame-count"))
    {
        state.framecount = j["frame-count"].get<std::uint32_t>();
    }

    if (j.contains("time-step"))
    {
        state.timestep = j["time-step"].get<std::uint32_t>();
    }

    if (j.contains("hitbox"))
    {
        state.hitbox = j["hitbox"].get<tt::HitBox>();
    }
}

void from_json(const nl::json& j, ItemCallbacks& cbs)
{
    if (j.contains("onSelect"))
    {
        j["onSelect"].get_to(cbs.onSelect);
    }
}

void from_json(const nl::json& j, ItemInstanceInfo& info)
{
    if (j.contains("x"))
    {
        if (j["x"].is_number())
        {
            info.x = j["x"].get<float>();
        }
        else if (j["x"].is_string()
            && j["x"].get<std::string>() == "random")
        {
            info.x = -1.f;
        }
        else
        {
            throw std::runtime_error("invalid item coordinate");
        }
    }

    if (j.contains("y"))
    {
        if (j["y"].is_number())
        {
            info.y = j["y"].get<float>();
        }
        else if (j["y"].is_string()
            && j["y"].get<std::string>() == "random")
        {
            info.y = -1.f;
        }
        else
        {
            throw std::runtime_error("invalid item coordinate");
        }
    }

    if (j.contains("respawn-delay"))
    {
        info.respawn = j["respawn-delay"].get<float>();
    }

    if (j.contains("scale"))
    {
        info.scale = j["scale"].get<sf::Vector2f>();
    }

    if (j.contains("default-state"))
    {
        info.defaultState = j["default-state"].get<std::string>();
    }

    if (j.contains("obtainable"))
    {
        info.obtainable = j["obtainable"].get<bool>();
    }

    info.callbacks = j.get<ItemCallbacks>();
}

namespace
{

int Item_getId(lua_State* L)
{
    auto item = tt::checkObject<Item>(L);
    lua_pushstring(L, item->getID().c_str());
    return 1;
}

int Item_getName(lua_State* L)
{
    auto item = tt::checkObject<Item>(L);
    lua_pushstring(L, item->getName().c_str());
    return 1;
}

int Item_getDescription(lua_State* L)
{
    auto item = tt::checkObject<Item>(L);
    lua_pushstring(L, item->description().c_str());
    return 1;
}

int Item_isObtainable(lua_State* L)
{
    auto item = tt::checkObject<Item>(L);
    lua_pushboolean(L, item->obtainable() ? 1 : 0);
    return 1;
}

int Item_setObtainable(lua_State* L)
{
    auto item = tt::checkObject<Item>(L);
    const auto val = lua_toboolean(L, 2);
    item->setObtainable(val);
    return 0;
}

int Item_setState(lua_State* L)
{
    auto item = tt::checkObject<Item>(L);
    const auto state = lua_tostring(L, 2);
    item->setBaseState(state);
    return 0;
}

}

const struct luaL_Reg Item::LuaMethods[] =
    {
        {"id", Item_getId},
        {"name", Item_getName},
        {"description", Item_getDescription},
        {"obtainable", Item_isObtainable},
        {"setObtainable", Item_setObtainable},
        {"setBaseState", Item_setState},
        {nullptr, nullptr}
    };

} // namespace tt

namespace std
{
    std::ostream& operator<<(std::ostream& out, const sf::Color color)
    {
        out << "{" << color.r << "," << color.g << "," << color.b << "," << color.a << "}";
        return out;
    }
}

namespace tt
{

Item::Item(const ItemInfo& obj, const ItemInstanceInfo& inst)
    : _objectInfo{ obj }, _instanceInfo{ inst }
{
    assert(obj.texture);
    _sprite.setTexture(*obj.texture);

    if (_objectInfo.size.has_value())
    {
        _size = *(_objectInfo.size);
        _sprite.setTextureRect(sf::IntRect(0, 0, _size.x, _size.y));
    }
    else
    {
        _size = _sprite.getTexture()->getSize();
    }

    std::string defaultState;
    if (_instanceInfo.defaultState.size() > 0)
    {
        defaultState = _instanceInfo.defaultState;
    }
    else if (_objectInfo.defaultState.size() > 0)
    {
        defaultState = _objectInfo.defaultState;
    }
    else if (_objectInfo.states.size() == 1)
    {
        // default to the only state
        defaultState = _objectInfo.states.begin()->first;
    }
    else if (_objectInfo.states.size() > 1)
    {
        // in this case the item has multiple states defined
        // but there is no default state, so we don't know
        // what to do
        throw std::runtime_error(fmt::format("object '{}' requires a 'default-state'", _objectInfo.id));
    }

    // now that we've defined the states, let's initiate the hitboxes, which
    // will be used for collision detection
    initStateHitboxes(defaultState);

    if (defaultState.size() > 0)
    {
        // `setBaseState` will set `_currentBaseState` for us
        setBaseState(defaultState);
    }
    else
    {
        // the idea here is that
        _currentBaseState = "@default";
    }

    _obtainable = _objectInfo.obtainable;
    if (_instanceInfo.obtainable.has_value())
    {
        _obtainable = *(_instanceInfo.obtainable);
    }

    _respawn = _objectInfo.respawn;
    if (_instanceInfo.respawn.has_value())
    {
        _respawn = *(_instanceInfo.respawn);
    }

    _highlight.setFillColor(sf::Color::Transparent);
    _highlight.setOutlineThickness(2);
    _highlight.setOutlineColor(sf::Color(255, 255, 255));
}

void Item::initStateHitboxes(const std::string &defaultstate)
{
    for (const auto& [key, value] : _objectInfo.states)
    {
        if (value.hitbox.has_value())
        {
            _hitboxes.emplace(key, *(value.hitbox));
        }
        else
        {
            // if no hitbox is defines, scan the first frame of the state to figure
            // out the bounds of the image
            auto image = this->_objectInfo.texture->copyToImage();
            std::optional<std::uint32_t> left, right, top, bottom;
            std::uint32_t x = 0;
            std::uint32_t y = 0;
            for (auto startX = (value.source.x * _size.x); startX < (value.source.x * _size.x) + _size.x; startX++, x++)
            {
                y=0;
                for (auto startY = (value.source.y * _size.y); startY < (value.source.y * _size.y) + _size.y; startY++, y++)
                {
                    sf::Color pixel = image.getPixel(startX, startY);
                    if (pixel.a == 0) continue;

                    if (!left.has_value() || x < *left)
                    {
                        left = x;
                    }

                    if (!top.has_value() || y < *top)
                    {
                        top = y;
                    }

                    if (!right.has_value() || x > *right)
                    {
                        right = x;
                    }

                    if (!bottom.has_value() || y > *bottom)
                    {
                        bottom = y;
                    }
                }
            }

            _hitboxes.emplace(key, HitBox{*left, *top, *right - *left, *bottom - *top });
        }
    }

    if (_hitboxes.size() == 0)
    {
        _hitboxes.emplace("@default", HitBox{ 0, 0, _size.x, _size.y });
    }
}

void Item::setBaseState(const std::string& statename)
{
    if (_objectInfo.states.find(statename) == _objectInfo.states.end())
    {
        throw std::runtime_error(
            fmt::format("object '{}' does not contain state '{}", this->objectInfo().id, statename));
    }

    if (_hitboxes.find(statename) == _hitboxes.end())
    {
        throw std::runtime_error(
                fmt::format("object '{}' missing hitbox for state '{}", this->objectInfo().id, statename));
    }

    _currentBaseState = statename;

    const auto& state = _objectInfo.states.at(statename);
    _source = state.source;

    _framecount = *(state.framecount);
    _timestep = *(state.timestep);

    if (_showHighlight)
    {
        auto width = _hitboxes.at(_currentBaseState).width * this->getScale().x;
        auto height = _hitboxes.at(_currentBaseState).height * getScale().y;
        _highlight.setSize(sf::Vector2f{
            static_cast<float>(width), static_cast<float>(height) });

        updateHighlight();
    }

    _sprite.setTextureRect(sf::IntRect(
        _source.x * _size.x, _source.y * _size.y, _size.x, _size.y));
}

void Item::queueState(const std::string& state)
{
    _stateQueue.emplace(state, tt::StateChangeType::QUEUE);
}

void Item::interruptState(const std::string& state)
{
    _stateQueue.emplace(state, tt::StateChangeType::INTERRUPT);
}

std::uint16_t Item::timestep()
{
    if (_framecount > 1
        && _timer.getElapsedTime().asMilliseconds() > static_cast<int>(_timestep))
    {
        auto[left, top] = _source;
        left++;

        auto textureWidth = _framecount > 0 ?
            _framecount * _size.x : _sprite.getTexture()->getSize().x;

        if (static_cast<std::uint32_t>(left * _size.x) >= textureWidth)
        {
            left = 0;
        }

        _source.x = left;
        _source.y = top;
        _sprite.setTextureRect(sf::IntRect(
            _source.x * _size.x, _source.y * _size.y, _size.x, _size.y));
        
        onFrameChange();

        _timer.restart();
    }

    return 0;
}

sf::FloatRect Item::getGlobalBounds() const
{
    const auto textureRect = _sprite.getTextureRect();
    float width = static_cast<float>(std::abs(textureRect.width));
    float height = static_cast<float>(std::abs(textureRect.height));
    return getTransform().transformRect(sf::FloatRect(0.f, 0.f, width, height));
}

sf::FloatRect Item::getGlobalHitBox() const
{
    sf::FloatRect retval;

    // TODO: this information could be cached each time the item moves
    const auto hpos = getPosition();
    retval.left = hpos.x + _hitboxes.at(_currentBaseState).left;
    retval.top = hpos.y + _hitboxes.at(_currentBaseState).top;
    retval.width = static_cast<float>(_hitboxes.at(_currentBaseState).width * this->getScale().x);
    retval.height = static_cast<float>(_hitboxes.at(_currentBaseState).height * this->getScale().y);

    return retval;
}

void Item::setHighlighted(bool h)
{
    _showHighlight = h;

    if (_showHighlight)
    {
        auto width = _hitboxes[_currentBaseState].width * this->getScale().x;
        auto height = _hitboxes[_currentBaseState].height * getScale().y;
        _highlight.setSize(sf::Vector2f{
            static_cast<float>(width), static_cast<float>(height) });

        updateHighlight();
    }
    else
    {
        _highlight.setSize(sf::Vector2f{ 0.f, 0.f });
    }
}

void Item::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(_sprite, states);
    target.draw(_highlight);
}

float Item::getGlobalLeft() const
{
    return getGlobalBounds().left;
}

float Item::getGlobalRight() const
{
    return getGlobalBounds().left + getGlobalBounds().width;
}

float Item::getGlobalTop() const
{
    return getGlobalBounds().top;
}

float Item::getGlobalBottom() const
{
    return getGlobalBounds().top + getGlobalBounds().height;
}

void Item::setGlobalLeft(float left)
{
    auto bounds = getGlobalBounds();
    setPosition(left, bounds.top);
    updateHighlight();
}

void Item::setGlobalRight(float right)
{
    auto bounds = getGlobalBounds();
    auto x = right - bounds.width;
    setPosition(x, bounds.top);
    updateHighlight();
}

void Item::setGlobalTop(float top)
{
    auto bounds = getGlobalBounds();
    setPosition(bounds.left, top);
    updateHighlight();
}

void Item::setGlobalBottom(float bottom)
{
    auto bounds = getGlobalBounds();
    auto y = bottom - bounds.height;
    setPosition(bounds.left, y);
    updateHighlight();
}

} // namespace tt
