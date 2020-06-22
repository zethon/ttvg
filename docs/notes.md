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

On macOS, passing `-r /some/folder` as a customer resource folder lead to the value of the folder having a leading space. This in turn caused all resources to fail loading.
