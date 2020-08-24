## Introduction

Lua support is being added to the Tommy Tooter Video Game to make it easier for the Tommy Tooter community to add content to the game. This document talks about the Lua API and gives examples of how to extend the game using Lua.

-- called for the 
function stripclub_onenter()
end

## Lua Files

Every scene has a *&lt;scene-name&gt;.lua* file which resides in the same folder as the map image and JSON config file. This Lua file contains all functions and code that is used by the scene and the scene's components. 

In addition to the scene itself, Lua events for zones and items are also supported.

### Configuring Functions

Functions are configured in the JSON like so:

```json
{
    "onInit": "scene_onInit()",
}
```

In this case, the scene will look in the corresponding *&lt;scene-name&gt;.lua* file for the functions. 

For example, if the above JSON was in the *EuclidHouse.json* file, then we could deduct 5% of the player's health every time they entered the scene by defining the following function in the *EuclidHouse.lua* file:

```lua
function scene_onEnter()
    local health = Player.getHealth()
    Player.setHealth(health - (health * 0.05))
end
```

It should be noted that the string passed to the callbacks in the JSON is interpreted as Lua, so it is possible to put Lua directly in the JSON file:

```json
{
    "onEnter": "local h=Player.getHealth();Player.setHealth(h-(h*0.05))"
}
```

## Scenes

There are three callback functions for every scene.

### `onInit()` *[callback]*

This function is invoked when the scene is first loaded. This should not be confused with when the scene is entered, which can happen multiple times during the game. `onInit` is called only at the start of the game, this can happen when the application is first loaded, or after a game has ended and a new one has begun.

### `onEnter` *[callback]*

### `onExit` *[callback]*


# Chapter 3. Resources

# Chapter 8. Modal Windows

 This library serves as means to display messages to the player such as dialog, or to prompt the player for questions. Modal Windows capture all input from the game demanding the user either make a choice or dismiss the window before moving on.

## 8.1. Static Modal Methods

Static methods can be used without declaring a `Modal` object, and provide basic functionality for common usages of modal windows.

### **`Modal.message(image,string)`** *[static]*

![](images/modal-dialogue.png)

Displays an image on the left hand side of the widow with a string on the right hand side. This box can be dismissed with the space bar or the escape key.

#### **Arguments**

`image` - A pre-cached image to display on the left hand side. This image will automatically be scaled to 32x32.</br>
`string` -  The string to display

<hr/>

### **`Modal.yesno(image,string)` &rarr; `bool`** *[static]*

![](images/modal-yesno.png)

Displays an image on the left hand side of the widow with a string on the right hand side. The player is presented with a yes/no selection that can be changed with the up and down arrow keys, and selected with the space bar.

on the left hand side of the window, and `string` on the right hand side. The `image` must have been previously loaded in the [resource cache](#resource-cache).

#### **Arguments**

`image` - A pre-cached image to display on the left hand side. This image will automatically be scaled to 32x32.</br>
`string` -  The string to display

#### **Returns**
`boolean` - Returns `true` if the user selected "Yes", or `false` if the user selected "No".

<hr/>

## 8.2. Modal Objects

When more fine grained control is needed, the `Modal` class offers more options. For example:

```lua
local window = Modal.new(nil, "Please select A, B or C")
window:addOption(0, "Choice A")
window:addOption(1, "Choice B")
window:addOption(2, "Choice C")

local result = window.exec()
if (result == 0) then
    Modal.message("You selected A")
else if (result == 1) then
    Modal.message("You selected B")
else if (result == 2) then
    Modal.message("You selected C")
end
```

### 11.2.4. `Modal.new(` Class

```lua
local window = Modal.new()

```
