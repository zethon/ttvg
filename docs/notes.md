## 2020-06-18

Trying to figure out a good architecture for the top level classes, but instead I should first I should do a basic design doc:

### Game Concept

The game will be a 2D scroller that allows X-Y movement around a small world. The player will interact with elements in the world, items and NPCs. Battles will consist of basic menu choices with each one having various consequences.

The Engine will consist of screens, where screens have one or more scenes. Namely only the "Game Screen" will have multiple scenes, which will change as the user navigates the world.

- Intro Screen
- Game Screen
- Finish Screen


## 2020-06-20

SFML does **NOT** support mp3 files (and CoolEdit does not support ogg files), so we'll use wav files.

In order for audio files to play on Windows I had to download the driver here: https://openal.org/downloads/

<s>For some reason Cool Edit's mp3->wav conversation doesn't work, so I've been using this website: https://audio.online-convert.com/convert-to-wav</s>

## 2020-06-21

Saving audio files in Cool Edit as "ACM Waveform" and unchecking "Save extra non-audio information" allows the audio files to load in SFML.

## 2020-06-22

On macOS, passing `-r /some/folder` as a custom resource folder lead to the value of the folder having a leading space (i.e. `[ /some/folder]`). This caused all resources to fail loading.

## 2020-06-26

My MBP will not load the .wav files saying it's a "unsupported format. I wasn't able to find anything definitive online but I'm guess it's because it's an older version of macOS (and for unrelated reasons, I cannot upgrade right now).

## 2020-06-27

I've started using the program here (https://github.com/colinvella/tIDE) to make words. It's a .NET map editor (Windows only) but is easy to use and not complidated.

## 2020-07-03

**TODO** - There needs to be some base class among drawable objects other than the SFML `Drawable` interface. Something needs to hold a reference to `ResourceManager` (why didn't I make this a singleton!?)

## 2020-07-04

* `VehicleFactory` should generate random vehicles on random paths at random intervals for random durations.
 
 ## 2020-07-05

 * `ResourceManager` should have type-specific version of some loaders (i.e. `loadFont()`) that then caches the resources.

## 2020-07-25 - Vehicles

Tecture images for vehicles must contain four rows of sprites, each row corresponds to the sprite shown when the car is moving in a particular direction. The rows are: DOWN, LEFT, RIGHT, UP. 

Here is how a vehicle is defined in a map's JSON config file:

```
"name" : "car1",
"size" : "77,41",
"tiles": "2,4",
"scale": "1.4,1.4",
"speed": "8.0,12.0"
 ```

 `name` - This is the base filename of the vehicle. This will resolve to `%resource-folder%/resources/textures/%name%.png`
 `size` : The size of each sprite within the text.
 `tiles` : The texture dimensions in terms of "tiles". For example, for the horizontal axis, this should be equal to width of the texture file divided by size.x. For the vertical axis, this should always be `4`.
 `scale` : The scaling factor applied to the sprites when drawn on the map.
 `speed` : The possible range of a vehicle's speed. 

 ### Speed

 The speed of the vehicle is measures in terms of pixels per timestep. However, the number of pixels is **not** the number of pixels drawn on the screen, which takes into account scrolling. The pixels are instead the numbe of pixels before any scaling is applied to the background.

