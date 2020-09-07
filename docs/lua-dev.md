<style type='text/css'>
.api-header 
{ 
    display:block; 
    font-family:"Lucida Console","Courier New";
    background: #F5F5F5;
    padding: 5px;
    border: 1px solid black;
}
</style>

## 0. Introduction

**This feature is a work in progress**. 

Lua support makes it easier for the community to add content to the game. This document talks about the Lua API and gives examples of how to extend the game using Lua.

Every scene has a *&lt;scene-name&gt;.lua* file which resides in `resources/lua` folder. This Lua file contains all code that is used by the scene and the scene's components. 

## 1. Scenes Events

Scenes will emit three events that can be caught in Lua. These are: `onInit`, `onEnter` and `onExit`. These are the default names and will be called automatically when the event is triggered. The names can be overriden using the JSON configuration like so:

```json
{
    "onInit": "scene_onInit",
    "onEnter": "scene_onEnter",
    "onExit": "scene_onExit",
}
```

Each event is passed a `Scene` object as the first parameter (see `Scene API` below).

A *Hello World* example using the configuration above might look like:

```lua
function scene_onInit(scene)
    print('Hello from '..scene:name..'!')
end

function scene_onEnter(scene)
    print('Entering scene '..scene:name..'!')
end

function scene_onExit(scene)
    print('Now leaving scene '..scene:name..'!')
end
```
</s>

### 1.1. Event Details

There are three callback functions for every scene.
<hr/>

#### `onInit(scene)`

This function is invoked when the scene is first loaded. This should not be confused with when the scene is entered, which can happen multiple times during the game. `onInit` is called only once when the game starts. Note that if game ends and the user starts a new game, `onInit` **will** be called at the start of that new game.

<hr/>

#### `onEnter(scene)`

This function is invoked when the player first enters the scene. This callback can be called multiple times in a single game since a user can enter and leave scenes multiple times.

<hr/>

#### `onExit(scene)`

This function is invoked when the playe first exits the scene. This callback can be also be called multiple times.

<hr/>

### 1.2. Scene API

#### `[void] Scene.addItem(item)`

The argument `item` is an `Item` object. This function adds an existing `Item` object to the current scene, hence the object must already exist when this function is called. 

For example:

```lua
function onEnter(scene)
    local item = ItemFactor.createItem("sax")
    scene:addItem(item)
end
```

<hr/>

#### `[DescriptionWindow] Scene.getDescriptionWindow()`

Returns a handle to the scene's description window. See "Description Window".

<hr/>

#### `[Player] Scene.getPlayer()`

No arguments. Returns a `Player` object. **This object is only valid for the life of the scene.** 

**Example**
```lua
function onEnter(scene)
    local player = scene:getPlayer()
    local health = player:getHealth() - 2
    player:setHealth(health)
end
```

<hr/>

#### `[string] Scene.name()`

No arguments. Returns the name of the current scene.

<hr/>

#### `[void] Scene.removeItem(item)`

The argument `item` is an `Item` object. If the scene does not contain the passed in item then nothing happens.

## 2. Player

The player object is accessible through `Scene.getPlayer`. **The player object is only valid for the life of the current scene.** This means that the player object is **not** available during initialization (i.e. `onInit`) since at that time there is no current scene.

This also means that storing the player object in an outer scope will not work. 

For example, **this will not work**:

```lua
gPlayer = nil

function onInit(scene)
    gPlayer = scene:getPlayer() -- 'gPlayer' is only valid within 'onInit'
end

function onEnter(scene)
    print("Health is "..gPlayer:getHealth()) -- CRASH!
end
```

Instead the player object must be retrieved from the scene object every time.

### 2.1 Player API


#### `[void] Player.addItem(item)`

Add the `item` object to `Player`'s inventory. `item` is an object and not a string value. 

Example:
```lua
-- give the player 'special-coin' when entering the scene
function scene_onEnter(scene)
    local item = ItemFactory.createItem('special-coin')
    player:addItem(item)
end
```

See more about "Items".
</span>

<hr/>

#### `[float] Player.getBalance()`
No arguments. Returns the value of the player's current balance.

<hr/>

#### `[float] Player.getBalance()`

Returns a float value of the player's current balance.

<hr/>

#### `[integer] Player.getHealth()`

Returns an intenger value of theplayer's current health.

<hr/>

#### `[bool] Player.hasItem(item)`

The argument `item` is an `Item` object (i.e. the type returned from `ItemFactory.createItem()`). Behind the scenes, `item` holds a pointer to the allocated item and does a raw pointer comparison with all the items contained in player inventory.

<hr/>

#### `[bool] Player.hasItemByName(itemid)`

The argument `itemid` is the **ID** of the item, which in most cases corresponds to the JSON filename of the item.

<hr/>

#### `[void] Player.removeItem(item)`

The argument `item` is a pointer to an `Item` object, just like the argument for `Player.hasItem`. 

<hr/>

#### `[bool] Player.removeItemByName(itemid)`

The argument `itemid` is the **ID** of the item, which in most cases corresponds to the JSON filename of the item.

<hr/>

#### `[void] Player.setBalance(balance)`

Sets the balance of the player to `balance`. This should be a numeric value.

<hr/>

#### `[void] Player.setHealth(health)`
Sets the health of the player to `health`. This should be an **integer** value.


## 3. Items

### 3.1 Item Events

Just like scenes, items have cofigurable events. The events are specified in the JSON file where each individual item is configured. 

For example:

```json
"treasure-chest":
[
    { 
        "x": 37, "y": 31, "onPickup": "treasureChest_onPickup" 
    }
],
```

These are the item events:

#### `[Item] onPickup(scene, item)`

`scene` is the current scene, and `item` is the item object being picked up.

<hr/>

### 3.2 ItemFactory


#### `[Item] ItemFactory.createItem(itemid)`

Parameter `itemid` is the item's unique key as determined by the filename. This function returns an `Item` object.

### 3.3 Item

#### `[bool] Item.actionable()`

<hr/>

#### `[string] Item.description()`

<hr/>

#### `[string] Item.id()`

<hr/>

#### `[string] Item.name()`

<hr/>

#### `[bool] Item.obtainable()`

<hr/>

#### `[void] Item.setActionable()`

<hr/>

#### `[void] Item.setDescription()`

<hr/>

#### `[void] Item.setName()`

<hr/>

#### `[void] Item.setObtainable()`

<hr/>

## 4. Description Window

<hr/>

#### `[string] DescriptionText.getText()`

<hr/>

#### `[void] ]DescriptionText.setText(text)`

<hr/>