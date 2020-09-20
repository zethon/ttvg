# Road Map

This document discusses long term plans for the game in the rough order in which I would like to tackle them. Of course these are tentative plans and are subject to change.

<hr/>

## Audio System Refactor

The system to play audio needs to be abstracted away from the SFML implementation. This will allow for things like log decoration, and a null audio system.

## Mission API

Need to a define a mission API.

<hr/>

## Dynamic Bounds Checking

Collision bounds need to be seperated from sprite bounds in a sprite sheet.

<hr/>

## Item General Callbacks

To hook into an Item event, the calback must be defined on every item configured in the scene's JSON file. For example:

```json
"sax"
[
    { "x":1, "y":1, "onPickup": "on_pickupSax" },
    { "x":10, "y":10, "onPickup": "on_pickupSax" },
    { "x":20, "y":20, "onPickup": "on_pickupSax" },
]
```

Instead there should be a way to define a catch-all and define a specific callback if needed. Something like:

```json
"sax"
[
    { "x":1, "y":1, "onPickup": "special_onPickupSax" },
    { "x":10, "y":10},
    { "x":20, "y":20},
    "onPickup": "generic_onPickupSax"
]
```

Such that picking up the first item would trigger `special_onPickupSax` but picking up any other instances would trigger `generic_onPickupSax`.

<hr/>

## Random Item Placement

Right now all items on a given map are in a fixed location. There should be a way to configure items such that they are placed randomly on the maps. Maybe this could be done by adding a `random` keyword in the `x`,`y` configuration, such that random items would be easily compatible with *Item General Callbacks*. For example:

```json
"sax"
[
    { "x":"random", "y":"random", "onPickup": "special_onPickupSax" },
    { "x":10, "y":10},
    { "x":20, "y":20},
],

```

<hr/>

## Respawning Items

### Motivation

There is functionality that allows Tommy to sell weed to an NPC. However, there are only two `bag-of-weed` items on the main map. There should be a way for items to respawn on the map either at the same spot or random spots. 

### Requirements

* set if the items should be replenished only after they've been used/picked up
* limit the number of items that can be on the map
* limit min number of items (i.e. the `bag-of-weed` might not respawn until Tommy has found them all)
* support random or static placement on the map
    * make sure not to place random items on already occupied spaces

<hr>

## Mac Installer

Like the Windows installer, Mac installers should be automatically deployed with pushes to master.

<hr/>

## AMB Badges lol

Acomplishments in the game could be rewarded with badges on AMB. This would require some kind of login in the game.

<hr/>

## Multiplayer! LOLLOL

Wouldn't it be great if multiple AMBers could wander around the same map and have hobofights?!

<hr/>

## MicroTransactions! LOLLLOLulz!

Wouldn't it also be great to send Max Power money?!
