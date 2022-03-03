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
    lua_pushstring(L, item->getDescription().c_str());
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
    item->setState(state);
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
        {"setState", Item_setState},
        {nullptr, nullptr}
    };


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
    else if (_objectInfo.states.size() > 0)
    {
        throw std::runtime_error(fmt::format("object '{}' requires a 'default-state'", _objectInfo.id));
    }

    if (defaultState.size() > 0)
    {
        _animated = true;
        setState(defaultState);
    }
    else
    {
        _hitbox = HitBox{ 0, 0, _size.x, _size.y };
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

void Item::setState(const std::string& statename)
{
    if (_objectInfo.states.find(statename) == _objectInfo.states.end())
    {
        throw std::runtime_error(
            fmt::format("object '{}' does not contain state '{}", this->objectInfo().id, statename));
    }

    const auto& state = _objectInfo.states.at(statename);
    _source = state.source;

    _framecount = *(state.framecount);
    _timestep = *(state.timestep);

    if (state.hitbox.has_value())
    {
        _hitbox = *(state.hitbox);
    }
    else
    {
        _hitbox = HitBox{ 0, 0, _size.x, _size.y };
    }

    _sprite.setTextureRect(sf::IntRect(
        _source.x * _size.x, _source.y * _size.y, _size.x, _size.y));
}

std::uint16_t Item::timestep()
{
    if (_animated
        && _framecount > 0
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

    auto hpos = getPosition();
    hpos.x += _hitbox.left;
    hpos.y *= _hitbox.top;
    _highlight.setPosition(hpos);

    return 0;
}

sf::FloatRect Item::getGlobalBounds() const
{
    const auto textureRect = _sprite.getTextureRect();
    float width = static_cast<float>(std::abs(textureRect.width));
    float height = static_cast<float>(std::abs(textureRect.height));
    return getTransform().transformRect(sf::FloatRect(0.f, 0.f, width, height));
}

void Item::setHighlighted(bool h)
{
    //if (!_animated)
    //{
    //    if (getID() == "@player")
    //    {
    //        std::cout << 23232;
    //    }

    //    // since we never call "setState" on unanimated object, we
    //    // have to set the hitbox here
    //    const auto rect = this->getGlobalBounds(); 
    //    auto width = static_cast<std::uint32_t>(rect.width);
    //    auto height = static_cast<std::uint32_t>(rect.height);
    //    if (this->objectInfo().hitbox.has_value())
    //    {
    //        width = this->objectInfo().hitbox->width * static_cast<std::uint32_t>(getScale().x);
    //        height = this->objectInfo().hitbox->height * static_cast<std::uint32_t>(getScale().y);
    //    }

    //    _hitbox = HitBox{0, 0, width, height};
    //}

    if (h)
    {
        //auto width = _hitbox.width * this-
        //auto height = _hitbox.height * getScale().y;
        //_highlight.setSize(sf::Vector2f{
        //    static_cast<float>(width), static_cast<float>(height) });
    }
    else
    {
        _highlight.setSize(sf::Vector2f{ 0.f, 0.f });
    }
}

void Item::setAnimated(bool v)
{
    _animated = v;
    _timer.restart();
}

void Item::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(_sprite, states);
    target.draw(_highlight);
}

} // namespace tt
