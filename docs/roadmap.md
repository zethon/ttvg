# Road Map

This document discusses long term plans for the game. 

These items are in a **rough order** in which I would like to tackle them. 

**All plans are tentative and subject to change.**

<hr/>

## Configurable Animation

Currently only the `Player` and `Vehicle` objects supported animation in a convoluted manner. This functionality should be refactored to allow for configurable animated states of all kinds of objects.

<hr/>

## Audio System Refactor

The system to play audio needs to be abstracted away from the SFML implementation. This will allow for things like log decoration, and a null audio system.

<hr/>

## Mac Installer

Like the Windows installer, Mac installers should be automatically deployed with pushes to master.

<hr>

## Dynamic Bounds Checking

Collision bounds need to be seperated from sprite bounds in a sprite sheet.

<hr/>

## Mission API

Need to a define a mission API.

<hr/>

## Integrate GUI Library

Explore GUI/Widget libraries that support SFLM. This will be useful for things like a Settings page where widgets (e.g. checkboxes, dropdowns) would be useful.

## Migrate to Github Actions

The Github action system supports CI testing on Windows, macOS and Ubuntu. We should migrate away from Travis-CI and AppVeyor and use the Github Action system.

<hr/>

## AMB Badges lol

Acomplishments in the game could be rewarded with badges on AMB. This would require some kind of login in the game.

<hr/>

## Multiplayer! LOLLOL

Wouldn't it be great if multiple AMBers could wander around the same map and have hobofights?!

<hr/>

## MicroTransactions! LOLLLOLulz!

Wouldn't it also be great to send Max Power money?!
