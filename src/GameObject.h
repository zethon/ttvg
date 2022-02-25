#pragma once
#include <functional>
#include <optional>

#include <boost/signals2.hpp>

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

#include "GameTypes.h"
#include "IUpdateable.h"

namespace nl = nlohmann;

namespace tt
{

constexpr auto DEFAULT_FRAMECOUNT = 1u;
constexpr auto DEFAULT_TIMESTEP = 55u;

class GameObject;
using GameObjectPtr = std::shared_ptr<GameObject>;

struct GameObjectInfo;
using GameObjectInfoPtr = std::shared_ptr<GameObjectInfo>;
using GameObjectInfoMap = std::map<std::string, GameObjectInfo>;
void from_json(const nl::json& j, GameObjectInfo& i);

struct GameObjectState;
using GameObjectStates = std::map<std::string, GameObjectState>;
void from_json(const nl::json& j, GameObjectState& i);

struct GameObjectCallbacks;
void from_json(const nl::json& j, GameObjectCallbacks& cbs);

struct GameObjectInstanceInfo;
using GameObjectInstanceInfoPtr = std::shared_ptr<GameObjectInstanceInfo>;
void from_json(const nl::json& j, GameObjectInstanceInfo& info);

struct GameObjectState
{
    std::string     id;
    sf::Vector2i    source;
    std::optional<std::uint32_t>    framecount;
    std::optional<std::uint32_t>    timestep;
};

struct GameObjectInfo
{
    std::string     id;             // unique! id is how the object is refrerenced in the json
    std::string     name;           // name is how the object shows up in the game (i.e. inventory)
    std::string     description;
    
    std::string     texturefile;
    sf::Texture*    texture = nullptr;

    // TODO (document): `size` here refers to the size of the individual
    // tiles inside the texture file, and *NOT* the size of the object
    // as it will be rendered in the scene
    std::optional<sf::Vector2u>     size;

    // default scale used for the object it the scale is not specified
    // in the Scene's settings
    std::optional<sf::Vector2f>     scale;

    std::string         defaultState;
    GameObjectStates    states;

    std::uint32_t   framecount = DEFAULT_FRAMECOUNT;
    std::uint32_t   timestep = DEFAULT_TIMESTEP;

    // refactored out?
    bool                obtainable = false;
};

// `GameObjectCallbacks` callbacks can be null or non-null and empty.
// If the callback is null, then it was not defined. If it is empty,
// then this denotes a configuration like: `"onSelect": ""` which
// might be used to override a default action with an empty action
struct GameObjectCallbacks
{
    // used when the item is picked up from the map
    std::optional<std::string> onSelect;

    // // used when a weapon is yielded or an instrument
    // // is played
    // std::optional<std::string> onUse;

    // // used when somethin is eaten, smoked, etc
    // std::optional<std::string> onConsume;
};

struct GameObjectInstanceInfo
{
    // a null x,y means that the coordinate was not specified,
    // and a value of -1 means it should be picked randomly
    std::optional<float>    x;
    std::optional<float>    y;
    std::optional<float>    respawn;

    std::optional<sf::Vector2f> scale;
    std::string                 defaultState;

    GameObjectCallbacks     callbacks;

    // will apply the default values if they are set in the `defaults` object
    // and *not* set in this object.
    // NOTE: We cannot use a constructor since these objects are constructed
    // through the JSON parser
    void applyDefaults(const GameObjectInstanceInfo& defaults)
    {
        if (!x.has_value() && defaults.x.has_value())
        {
            x = defaults.x;
        }

        if (!y.has_value() && defaults.y.has_value())
        {
            y = defaults.y;
        }

        if (!respawn.has_value() && defaults.respawn.has_value())
        {
            respawn = defaults.respawn;
        }

        if (!scale.has_value() && defaults.scale.has_value())
        {
            scale = defaults.scale;
        }

        if (!callbacks.onSelect.has_value() && defaults.callbacks.onSelect.has_value())
        {
            callbacks.onSelect = defaults.callbacks.onSelect;
        }

        if (defaultState.size() == 0 && defaults.defaultState.size() > 0)
        {
            defaultState = defaults.defaultState;
        }
    }
};

class GameObject :
    public sf::Drawable,
    public sf::Transformable,
    public IUpdateable
{

public:

    GameObject(const GameObjectInfo& info, const sf::Texture& texture);
    GameObject(const GameObjectInfo& obj, const GameObjectInstanceInfo& inst);

    const GameObjectCallbacks& callbacks() const
    {
        return this->_instanceInfo.callbacks;
    }

    const GameObjectInfo& objectInfo() const
    {
        return this->_objectInfo;
    }

    const GameObjectInstanceInfo& instanceInfo()
    {
        return this->_instanceInfo;
    }

    void setState(const std::string& statename);
    
    void setHighlighted(bool h);
    bool highlighted() const { return _highlight.getSize().x != 0; }
    
    sf::RectangleShape& highlight() { return _highlight; }

    sf::FloatRect getGlobalBounds() const;

    std::uint16_t timestep() override;

    bool animated() const { return _animated; }
    void setAnimated(bool v);

public: // signals
    
    boost::signals2::signal<void(void)> onFrameChange;

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;

    sf::Vector2u    _size; // fixed cell size of each frame within the sprite 
    sf::Clock       _timer;

    // some sprite sheets have different frames per row
    // so this allows us to adjust how many frames get
    // animated in a particular row
    std::uint32_t   _framecount = 0;                // of current state
    std::uint32_t   _timestep = DEFAULT_TIMESTEP;   // of current state
    sf::Vector2i    _source;                        // of current state

    sf::Sprite          _sprite;
    sf::RectangleShape  _highlight;

    bool                _animated = false;
    

    // 2022-02-10: The idea right now is that a `GameObject` has a reference
    // to a `GameObjectInfo` structs, and to also a `GameObjectInstance` struct
    // to let it know about the instance of the `GameObject`
    const GameObjectInfo&           _objectInfo;    // ref to the cached info
    const GameObjectInstanceInfo    _instanceInfo;  // copy of the instance info
};

} // namespace tt
