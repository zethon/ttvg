# Road Map

This document discusses long term plans for the game in the order in which I would like to tackle them. Of course these are tentative plans and are subject to change.

## Modal Windows (in progress)

Modal windows need a way to:

* display dialog across multiple scenes
* get user selections from a list of options
* be completely accessible through the Lua API

## Windows Installer

The Windows installer should automatically build when code is pushed to master and automatically deploy the installer somewhere for download.

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

## Random Item Placement

Right now all items on a given map are in a fixed location. There should be a way to configure items such that they are placed randomly on the maps. Maybe this could be done by adding a `random` keyword in the `x`,`y` configuration, such that random items would be easily compatible with *Item General Callbacks*. For example:

```json
"sax"
[
    { "x":"random", "y":"random", "onPickup": "special_onPickupSax" },
    { "x":10, "y":10},
    { "x":20, "y":20},
    "onPickup": "generic_onPickupSax"
]
```

## Mac Installer

Mac users need love too! Like the Windows installer, Mac installers should be automatically deployed with pushes to master.

## AMB Badges

Acomplishments in the game could be rewarded with badges on AMB. This would require some kind of login in the game.

## Multiplayer!

Wouldn't it be great if multiple AMBers could wander around the same map and have hobofights?!

## MicroTransactions! LOL

Wouldn't it also be great to send Max Power money?!
