 # Scenes

 ## Intro

The game is divided into multiple screens that represent different stages in the interaction of the program, for example there is the splash screen, the menu screen and, of course, the game screen. The `Screen` class is the base class of all screens in the game.

The most import screen is `GameScreen` which contains the actual game play. Game play itself is further divided into mutiple scenes.

The `TooterEngine` class is responsible for controling the active screen, and the `GameScreen` class is reponsible for controling the current screen.

## Adding Scenes

Scenes are either a class derived from `Scene` or an actual `Screen` object. The `GameScreen` class has a `std::map` that contains the name of each `Scene` and a shared pointer to the object itself. Too add a new scene to the game we simply create the `Scene` object and add it to `GameScreen::_scences`:

```
_scenes.emplace("tucson", std::make_shared<Opening>(resmgr, target, _player));
_scenes.emplace("EuclidHouse", std::make_shared<Scene>("EuclidHouse", resmgr, target, _player));
```

In the above example, we have added two scenes. The first is of type `Opening` which is derived from `Scene`. We do not have to pass in the name of the `Opening` object since that is taken care of in the constructor:

```
constexpr auto SCENE_NAME = "tucson";

Opening::Opening(ResourceManager& resmgr, sf::RenderTarget& target, PlayerPtr player)
    : Scene{ SCENE_NAME, resmgr, target, player },

```

However, if the scene doesn't need any custom behavior, then using the `Scene` class should suffice for most purposes. 

## Scene Name

The scene name is how the game engine references the scene in various contexts. There are two primary places where this is used.

### Scene Image

The background imagine used for a scene is defined by the scene's name. The image is a ".png" file that is contained in the `resources/maps` folder. For example, the "EuclidHouse" scene's background image is `resources/maps/EuclidHouse.png`. 

**Note:** Currently the filename of the image is fixed and cannot be changed through configuration.

### Scene Configuration

When a `Scene` object is constructed, a corresponding JSON configuration file is loaded. The configuration filename is the same as the scene's name in the `/resources/maps` folder. For example, the "EuclidHouse" scene in our example above has a configuation file of `resources/maps/EuclidHouse.json` (the filenames **are** case sensative). 

The configuration file defines several things including:

* background image scaling, and tilesize
* player object attributes for the screen 
* zone information
* item information

### Background Information

The background for each scene can have a custom scaling of the image, and each scene can have a custom tilesize. 

```
"background":
{
    "scale": { "x": 2.25, "y": 2.25 },
    "tiles": { "x": 16, "y": 16 }
}
```

* `scale` - Defines the scaling for the background image
* `tiles` - The tilesize for the map. For now this is always 16x16.

### Player Information

The player object is shared among all scenes, however each scene can customize how the player is drawn. When a scene is entered (not loaded), the scene's `Scene::enter()` method is called (likewise there is a corresponding `Scene::exit()` method). This allows the scene to change any attributes about the player object. 

A consequence of each scene having its own settings for the player means that **every scene must define these settings**, else settings might propagate across scenes which might result in things looking weird.

```
"player":
{
    "start": { "x": 1660, "y": 2855 },
    "scale": { "x": 1.5, "y": 1.5 },
    "source": { "x": 0, "y": 10 },
    "origin": { "x": 0, "y": 0 },
    "stepsize" : 16.0
}
```

* `start` - This defines the starting position of the player **the first time the player enteres the scene**. When a player leaves a scene, the exact coordinates of their leaving position are stored and then restored in subsequent visits to the scene. These coordinates are defined in terms of global coordinates. 
* `scale` - The scaling of the player image. Larger maps may want to make the player avatar appear smaller (e.g. the `tucson` scene), whereas smaller maps might want to make the player avatar larger (e.g. the `EuclidHouse` scene).
* `source` - These coordinates define the starting sprite image of the player when entering the scene. These coordinates are in terms of row and column of the player's sprite sheet (`resources/images/tommy-1.png`). 
* `origin` - **Should always be (0,0) for now** 
* `stepsize` - This defines how "fast" the player walks and is defined in terms of "pixels per game loop". 

### Zones

The zones define rectangular blocks on the map that trigger certain actions. The most common action is to simply display a string showing the player where they are.

```
{ 
    "name" : "Home",
    "description": "Press SPACE to enter your filthy apartment",
    "rects" :
    [ 
        "48,77,52,80", 
        "53,77,54,79"
    ],
    "transition":
    {
        "scene": "EuclidHouse",
        "enabled": true
    }
}
```

* `name` - This is the name of the zone that is displayed in the user's HUD.
* `description` - This text is displayed just below the user's HUD. This is a good place to give the user instructions (e.g. "Press SPACE to enter")
* `rects` - This is an array of strings that are parsed into rectangles. Unlike SFML rects which are defined as `(top, left, width, height)`, these rectangles are defined as `(x1, y1, x2, y2)`. Configuration can define one or more rectangles within this array.
* `transition` - This argument is optional. This settings allows the user to load another scene by pressing the space bar. 
    * `scene` - The name of the scene to be entered. 
    * `enabled` - **Currently not supported**


## Todo

* Give zones definable `id` attributes. This will allow for code to do things such as enable and disable transitions, or change the name or description based on certain conditions.

* The `source` field in the player configuration should be supported, but first there needs to be consistency to how it's used. This field allows you to define how the object is referenced in the world. For example, `player->setPosition(1,1)` will set the position of the center of the player avatar at `(1,1)`, **however** if you do:

```
player->setSource(0,0);
player->setPosition(1,1);
```

That will then set the upper left corner of the player avatar at `(0,0)`. 

