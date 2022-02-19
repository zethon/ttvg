# Objects

Game objects can be placed anywhere on a scene/map using some JSON configuration. A _game object_ is any object in the game with which the player can interact, whether an item the player can pick up, or an NPC the player can fight. 

## Example 1 - Plastic Bag

For the first example we will define a plastic bag that appears on the map that a player can pick up. First we start with defining the JSON file that describes the plastic bag. We create a file named `plastic-bag.json` inside the `<project-root>/resources/items` folder (i.e. `<project-root>/resources/plastic-bag.json`). Inside that file we put the following:

```json
{
    "name": "Plastic Bag",
    "texture": "items/plastic-bag.png",
    "obtainable": true,
}
```

MESSY NOTE: Settings have a heirachy. For example, 'scale' can be defined in the object's .json file, this `scale` will be override by a `scale` defined in the `Scene`'s instance info for that object, and that `scale` can be overriden by the `scale` defined in specific `instance` object.


For example, say we want to add a pair of handcuffs inside our police station that the player can pickup. To add the "handcuffs" we:

* Add the item .png file to *resources/items* (i.e. *resources/items/handcuffs.png*)
* Create a configuration item for the item (i.e. *resources/items/handcuff.json*). 
* Add the item to the PoliceStation's configuration

```json
"items":
{
    "handcuffs":
    {
        "instances":
        [    
            { "x": 5, "y": 5 }
            { "x": 24, "y": 32 }
        ]
    }
}
```

The item name should be used as the JSON key for the item (in this case "handcuffs"). For each pair of coordinates in the array, and item will be placed at that location. The coordinates are defined in terms of tile coordinates.

### Item Defaults

Items can have default settings that apply to each instance of the item in the scene. The default settings can also be overridden by a specifc instance. For example:

```json
"items":
{
    "handcuffs": 
    {
        "onSelect": "handcuff_pickup",
        "instances":
        [    
            { "x": "random", "y": "random" },
            { "x": "random", "y": "random" },
            { "x": "random", "y": "random" },
            { "x": 24, "y": 32, "onSelect": "special_handcuffs" }
        ]
    }
}
```

In the above example, each instance of `handcuffs` will call `handcuff_pickup` in the Lua system when the item is picked up by the player, except for the last instance which will instead call `special_handcuffs`. A specific instance can also override the callback to a null action by doing `"onSelect": ""`.

### Randomizing Item Location

Items can be randomly placed on a map by using the `random` keyword in the configuration. For example:

```json
"items":
{
    "handcuffs": 
    {
        "instances":
        [    
            { "x": "random", "y": "random" }
            { "x": 24, "y": 32 }
        ]
    }
}
```

The `random` keyword can be used on both the `x` and `y` coordinates, or just one.

### Respawning Items

Items can be set to respawn a specific amount of time after the player has picked it up.

```json
"items":
{
    "handcuffs": 
    {
        "respawn-delay": 60,
        "instances":
        [    
            { "x": "random", "y": "random" }
        ]
    }
}
```

Here we have configured the set of handcuffs to respawn 60 seconds after the user has picked it up. Note that a random location for the handcuffs will be chosen every time the item spawns.