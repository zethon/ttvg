#pragma once
#include <functional>
#include <optional>
#include <queue>

#include <boost/signals2.hpp>

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <lua.hpp>

#include "GameTypes.h"
#include "IUpdateable.h"
#include "TooterLogger.h"

namespace nl = nlohmann;

namespace tt
{

constexpr auto DEFAULT_FRAMECOUNT = 1u;
constexpr auto DEFAULT_TIMESTEP = 55u;

class Item;
using ItemPtr = std::shared_ptr<Item>;

struct ItemInfo;
using ItemInfoPtr = std::shared_ptr<ItemInfo>;
using ItemInfoMap = std::map<std::string, ItemInfo>;
void from_json(const nl::json& j, ItemInfo& i);

struct ItemState;
using ItemStates = std::map<std::string, ItemState>;
void from_json(const nl::json& j, ItemState& i);

struct ItemCallbacks;
void from_json(const nl::json& j, ItemCallbacks& cbs);

struct ItemInstanceInfo;
using ItemInstanceInfoPtr = std::shared_ptr<ItemInstanceInfo>;
void from_json(const nl::json& j, ItemInstanceInfo& info);

using HitBox = sf::Rect<std::uint32_t>;

struct ItemState
{
    std::string     id;
    sf::Vector2i    source;
    std::optional<std::uint32_t>    framecount;
    std::optional<std::uint32_t>    timestep;
    std::optional<tt::HitBox>       hitbox;
};

struct ItemInfo
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

    std::string     defaultState;
    ItemStates      states;

    std::uint32_t           framecount = DEFAULT_FRAMECOUNT;
    std::uint32_t           timestep = DEFAULT_TIMESTEP;
    std::optional<HitBox>   hitbox;
    bool                    obtainable = false;
    float                   respawn = 0.f;
};

// `ItemCallbacks` callbacks can be null or non-null and empty.
// If the callback is null, then it was not defined. If it is empty,
// then this denotes a configuration like: `"onSelect": ""` which
// might be used to override a default action with an empty action
struct ItemCallbacks
{
    // used when the item is picked up from the map
    std::optional<std::string> onSelect;

    // // used when a weapon is yielded or an instrument
    // // is played
    // std::optional<std::string> onUse;

    // // used when somethin is eaten, smoked, etc
    // std::optional<std::string> onConsume;
};

struct ItemInstanceInfo
{
    std::string objid; // the id of the object this is an instance of

    // a null x,y means that the coordinate was not specified,
    // and a value of -1 means it should be picked randomly
    std::optional<float>    x;
    std::optional<float>    y;
    std::optional<float>    respawn;

    std::optional<sf::Vector2f> scale;
    std::string                 defaultState;
    std::optional<bool>         obtainable;

    ItemCallbacks         callbacks;

    // will apply the default values if they are set in the `defaults` object
    // and *not* set in this object.
    // NOTE: We cannot use a constructor since these objects are constructed
    // through the JSON parser
    void applyDefaults(const ItemInstanceInfo& defaults)
    {
        // this allows us to set the objid once when loading the Scene
        // and then just copy it to the individual instances
        if (objid.size() == 0 && defaults.objid.size() > 0)
        {
            objid = defaults.objid;
        }

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

        if (!obtainable.has_value() && defaults.obtainable.has_value())
        {
            obtainable = defaults.obtainable;
        }

        if (!callbacks.onSelect.has_value() && defaults.callbacks.onSelect.has_value())
        {
            callbacks.onSelect = defaults.callbacks.onSelect;
        }

        if (defaultState.size() == 0 && defaults.defaultState.size() > 0)
        {
            defaultState = defaults.defaultState;
        }

        if (!respawn.has_value() && defaults.respawn.has_value())
        {
            respawn = defaults.respawn;
        }
    }
};

class Item :
    public sf::Drawable,
    public sf::Transformable,
    public IUpdateable
{

public:
    static constexpr auto CLASS_NAME = "Item";
    static const struct luaL_Reg LuaMethods[];

    Item(const ItemInfo& obj, const ItemInstanceInfo& inst);

    const ItemCallbacks& callbacks() const
    {
        return this->_instanceInfo.callbacks;
    }

    const ItemInfo& objectInfo() const
    {
        return this->_objectInfo;
    }

    const ItemInstanceInfo& instanceInfo()
    {
        return this->_instanceInfo;
    }

    float getGlobalLeft() const;
    float getGlobalRight() const;
    float getGlobalTop() const;
    float getGlobalBottom() const;

    void setGlobalLeft(float left);
    void setGlobalRight(float right);
    void setGlobalTop(float top);
    void setGlobalBottom(float bottom);

    void setBaseState(const std::string& statename);
    void queueState(const std::string& state);
    void interruptState(const std::string& state);

    void setHighlighted(bool h);
    bool highlighted() const { return _highlight.getSize().x != 0; }
    
    sf::RectangleShape& highlight() { return _highlight; }
    sf::FloatRect getGlobalBounds() const;
    sf::FloatRect getGlobalHitBox() const;
    std::uint16_t timestep() override;

    bool obtainable() const { return _obtainable; }
    void setObtainable(bool o) { _obtainable = o; }

    float respawn() const { return _respawn; }
    void setRespawn(float r) { _respawn = r; }

    std::string getID() const
    {
        return this->_objectInfo.id;
    }

    std::string getName() const
    {
        return _objectInfo.name;
    }

    std::string description() const
    {
        return _objectInfo.description;
    }

    int luaIdx() const { return _luaIdx; }
    void setLuaIdx(int i) { _luaIdx = i; }

    void updateHighlight()
    {
        auto hpos = getPosition();
        hpos.x += (_hitboxes.at(_currentBaseState).left * getScale().x);
        hpos.y += (_hitboxes.at(_currentBaseState).top * getScale().y);
        _highlight.setPosition(hpos);
    }

public: // signals
    
    boost::signals2::signal<void(void)> onFrameChange;

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void initStateHitboxes();

    const ItemInfo&           _objectInfo;    // ref to the cached info
    const ItemInstanceInfo    _instanceInfo;  // copy of the instance info

    log::SpdLogPtr      _logger;

    sf::Sprite          _sprite;
    sf::Vector2u        _framesize;          // fixed cell size of each frame within the sprite
    sf::Clock           _timer;
    int                 _luaIdx = 0;    // all items must be registered with the Lua system

    std::string         _currentBaseState;
    const ItemState*    _currentState = nullptr;    // current state being displayed
    std::uint32_t       _currentFrame = 0;          // current frame of the current state

    // we refer to the state map through a pointer, this way we do not require that
    // static Items have a state map because instead we can create a "default" state
    // map and point to it
    const ItemStates*   _states = nullptr;          // a reference to the active state map
    ItemStates          _defaultStates;             // used when item has no defined states

    std::map<std::string, HitBox>   _hitboxes;
    sf::RectangleShape              _highlight;

    std::queue<std::string> _stateQueue;
    std::string             _stateInterrupt;

    bool            _obtainable = false;
    float           _respawn = 0.0;
    bool            _showHighlight = false;
};

} // namespace tt
