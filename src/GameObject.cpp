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

    if (j.contains("states")
        && j["states"].is_array())
    {
        i.states.emplace();

        for (const auto& item : j["states"].items())
        {
            auto state = item.value().get<GameObjectState>();
            i.states->insert(std::make_pair(state.id, std::move(state)));
        }
    }

    if (j.contains("size"))
    {
        i.size = j["size"].get<sf::Vector2u>();
    }

    if (j.contains("scale"))
    {
        i.scale = j["scale"].get<sf::Vector2f>();
    }

    if (j.contains("framecount"))
    {
        i.framecount = j["framecount"].get<std::uint32_t>();
    }

    if (j.contains("timestep"))
    {
        i.timestep = j["timestep"].get<std::uint32_t>();
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

    info.callbacks = j.get<GameObjectCallbacks>();
}

GameObject::GameObject(const GameObjectInfo& info, const sf::Texture& texture)
    : _objectInfo{info}
{
    _sprite.setTexture(texture);

    if (info.size.has_value())
    {
        _size = *info.size;
        _sprite.setTextureRect(sf::IntRect(0, 0, _size.x, _size.y));
    }
    else
    {
        _size = _sprite.getTexture()->getSize();
    }

    if (info.scale.has_value())
    {
        setScale(*(info.scale));
    }

    std::uint32_t count = 1;
    if (info.framecount.has_value())
    {
        count = *(info.framecount);
    }

    if (info.states.has_value())
    {
        // TODO: container copy!!!
        for (const auto& [id, state] : *(info.states))
        {
            GameObjectState newstate = state;
            if (!newstate.framecount.has_value()) newstate.framecount = count;
            _states.emplace(id, std::move(newstate));
        }
    }
    else
    {
        _states.insert(
            std::make_pair("default", GameObjectState{ "default", sf::Vector2i{0,0}, 1 }));
    }

    _highlight.setFillColor(sf::Color::Transparent);
    _highlight.setOutlineThickness(2);
    _highlight.setOutlineColor(sf::Color(255, 255, 255));
}

GameObject::GameObject(const GameObjectInfo& obj, const GameObjectInstanceInfo& inst)
    : _objectInfo{ obj }, _instanceInfo{ inst }
{
    assert(obj.texture);
    _sprite.setTexture(*obj.texture);

    _size = _sprite.getTexture()->getSize();

    _highlight.setFillColor(sf::Color::Transparent);
    _highlight.setOutlineThickness(2);
    _highlight.setOutlineColor(sf::Color(255, 255, 255));
}

void GameObject::setState(const std::string& statename)
{
    const auto& state = _states.at(statename);
    _source = state.source;
    _sprite.setTextureRect(sf::IntRect(
        _source.x * _size.x, _source.y * _size.y, _size.x, _size.y));
}

std::uint16_t GameObject::timestep()
{
    if (animated()
        && _timer.getElapsedTime().asMilliseconds() > static_cast<int>(_timestep))
    {
        auto[left, top] = _source;
        left++;

        auto textureWidth = _maxFramesPerRow > 0 ?
            _maxFramesPerRow * _size.x : _sprite.getTexture()->getSize().x;

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
