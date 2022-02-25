#include <iostream>

#include <fmt/core.h>

#include "GameObject.h"
#include "TTUtils.h"

namespace nl = nlohmann;

namespace tt
{

void from_json(const nl::json& j, GameObjectInfo& i)
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

    if (j.contains("states") && j["states"].is_array())
    {
        i.states.emplace();

        for (const auto& item : j["states"].items())
        {
            auto state = item.value().get<GameObjectState>();
            if (!state.framecount.has_value())
            {
                state.framecount = i.framecount;
            }

            if (!state.timestep.has_value())
            {
                state.timestep = i.timestep;
            }

            i.states.insert(std::make_pair(state.id, std::move(state)));
        }
    }
}

void from_json(const nl::json& j, GameObjectState& state)
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
}

void from_json(const nl::json& j, GameObjectCallbacks& cbs)
{
    if (j.contains("onSelect"))
    {
        j["onSelect"].get_to(cbs.onSelect);
    }
}

void from_json(const nl::json& j, GameObjectInstanceInfo& info)
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

    info.callbacks = j.get<GameObjectCallbacks>();
}

GameObject::GameObject(const GameObjectInfo& obj, const GameObjectInstanceInfo& inst)
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

    _highlight.setFillColor(sf::Color::Transparent);
    _highlight.setOutlineThickness(2);
    _highlight.setOutlineColor(sf::Color(255, 255, 255));
}

void GameObject::setState(const std::string& statename)
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

    _sprite.setTextureRect(sf::IntRect(
        _source.x * _size.x, _source.y * _size.y, _size.x, _size.y));
}

std::uint16_t GameObject::timestep()
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

    _highlight.setPosition(getPosition());
    return 0;
}

sf::FloatRect GameObject::getGlobalBounds() const
{
    const auto textureRect = _sprite.getTextureRect();
    float width = static_cast<float>(std::abs(textureRect.width));
    float height = static_cast<float>(std::abs(textureRect.height));
    return getTransform().transformRect(sf::FloatRect(0.f, 0.f, width, height));
}

void GameObject::setHighlighted(bool h)
{
    if (h)
    {
        _highlight.setPosition(getPosition());
        auto b = getGlobalBounds();
        _highlight.setSize(sf::Vector2f{ b.width, b.height });
    }
    else
    {
        _highlight.setSize(sf::Vector2f{ 0.f, 0.f });
    }
}

void GameObject::setAnimated(bool v)
{
    _animated = v;
    _timer.restart();
}

void GameObject::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(_sprite, states);
    target.draw(_highlight);
}

} // namespace tt
