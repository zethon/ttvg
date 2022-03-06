# Items

Items, some times called _game objects_,  can be placed anywhere on a scene  using some JSON configuration. An item can refer to any object in the game with which the player can interact in some way. Items can be simple items like a _plastic bag_ on the side of the road that the player can pick up, or a fire that is animated like you would expect a fire to be. Items can also be an NPC with which the player to can talk to.

## Item Components

Items are defined by three components:

* The descriptor file
* The image file
* The instance information in the scene file

## The Descriptor File

The descriptor file is a JSON file that resides in the `resources/items` folder. This file is used to describe general attributes of the item, such as its name, description, whether or not it is animated, the states it has, etc. 

A simple example is a plastic bag that appeears in the Tuscon map at the start of the game. It's not animated, nor does its state ever change.

For example, we can create a `resources/items/bag.json` file to describe a plastic bag like so:

```json
{
    "name":         "Bag",
    "description":  "This bag is empty, but it is full of potential.",
    "texture": "items/bag.png",
    "obtainable": false
}
```

For obtainable items, the `name` field sets how the item will appear throughout the game, for example in the user's inventory. The `description` field will be displayed in the HUD when the player walks near the item. The `texture` field is the items image file, and the field `obtainable` defines whether or not the player can pickup this item and add it to their inventory.

If the `texture` field is left out, then the engine will automatically look in the items folder for a `.png` file with the same name as the `.json` file. Hence, that field could be left out in this example. 

Likewise, all items default to **not** being obtainable, hence the `obtainable` field could also be left out, and this item could simply be defined as:

```json
{
    "name":         "Bag",
    "description":  "This bag is empty, but it is full of potential."
}
```

## The Image File

The image file must be a PNG file with a transparent background. As we get into more complicated examples we will talk more about the layout of images in order to create animated items.

## Instance Information

Now that we have defined the item we need to let the engine know where we want the items placed. This can be done by defining the instance information of an item in a scene file. 

Every scene in the game has a `.json` file, and inside of every scene file is an `items` section that contains a list of all the items in that scene. For example, if we open up the `resources/maps/Tucson.json` file, we can see in the `items` section a section for the `bag`:

```json
"items":
{
    ...
    "bag":
    {
        "instances":
        [
            { "x": 34, "y": 85 },
            { "x": 91, "y": 77 },
            { "x": 91, "y": 92 }
        ]
    },
    ...
}
```

Here the `bag` refers to the name of the `.json` file in `resources/items`. In other words, the engine will append `.json` to the text `bag` (i.e. `bag.json`) and then look for a file with that name in the `resource/items` folder. In this case, `bag` is referred to the item's **ID**.

The `instances` array defines all the coordinates where a `bag` will appear. These coordinates are in terms of the background's tiles.

## Item Sizes

In none of the files did we define the size of the plastic bag. If we look at the image itself we can see its dimensions are 669x649, yet the bag appears very small on the map itself. 

By default the engine will scale all items to be the same size as a tiles of the scene, which is set in the scene's information (see the scene documentaion for more information about this). However we can override this scaling by defining the scaling in the instance information of the item. For example:

```json
"items":
{
    ...
    "bag":
    {
        "scale": { "x": 0.5, "y": 0.25 },
        "instances":
        [
            { "x": 34, "y": 85 },
            { "x": 91, "y": 77 },
            { "x": 91, "y": 92 }
        ]
    },
    ...
}
```

In the above case the item will be scaled such that the width of the item is half the original size, and the height of the item is a quarter of the original size. Figuring out the correct scaling for a item can take some trial and error to figure out whaat works best. 

Also above we have defined the scaling for every instance of the item in the scene, however it is possible to set the scaling of one of more particular instances by use of an _override_. 

```json
"items":
{
    ...
    "bag":
    {
        "scale": { "x": 0.5, "y": 0.25 },
        "instances":
        [
            { "x": 34, "y": 85, "scale": { "x": 1.5, "y": 1.25 } },
            { "x": 91, "y": 77 },
            { "x": 91, "y": 92 }
        ]
    },
    ...
}
```

Here the item at `(34, 85)` will be much bigger than the other two instances. This is example of how attributes can be used for all instances, but can be overridden for one or more instances. There are several attributes that support this. 




First we start with defining the JSON file that describes the plastic bag. We create a file named `plastic-bag.json` inside the `<project-root>/resources/items` folder (i.e. `<project-root>/resources/plastic-bag.json`). Inside that file we put the following:

```json
{
    "name": "Plastic Bag",
    "texture": "items/plastic-bag.png",
    "obtainable": true,
}
```
<hr/>

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