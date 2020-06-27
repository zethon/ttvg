# The Tommy Tooter Video Game

This is a 2D RPG game written in C++. It uses the [SFML](http://sfml-dev.org/) library for multimedia, and supports Winows and Mac.

## How to Install

Right now there is no installer, so the game has be built manually. An installer for both Windows and Mac will be created as the game matures and nears a releaseable state.

## How to Build

### Windows

I have been using Visual Studio 2017 as my toolchain, even tough I've actually been using Visual 2019 as my editor/IDE. I've yet to figure out how to get **conan** to play nicely with Visual Studio 2019, but since VS2019 can load and use VS2017 solution files without a problem, this method works fine.

#### Requirements
* Visual Studio 2017
* CMake 3.10+
* conan 1.21+
* OpenAL Libraries (https://openal.org/downloads/)

#### Instructions

(Note: these instructions may be _Linux'y_ since I use the bash shell on Windows more than I use PowerShell or cmd.exe)

* Download the source code
```
git clone https://github.com/lulzapps/ttvg.git
```
* Create and cd into a build directory: 
```
mkdir build 
cd build
```
* Initialize conan to install and build the required packages: 
```
conan install ../ttvg -s build_type=Debug --build mising
```
* Initialize CMake: 
```
cmake .. "-GVisual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Debug
```

If all goes well you should have a **ttvg.sln** in the `build` folder. This is a Visual Studio solution file that can be loaded and you're ready to start coding.

### Mac

I do not use Xcode because I hate it. However, if you want to use Xcode than you should just have to change the generator (the `-G` argument) in the CMake command (see the Windows instructions for an example of how to specify the generator).

#### Requirements
* clang (any version that supports C++17)
* CMake 3.10+
* conan 1.21+

#### Instructions

* Download the source code
```
git clone https://github.com/lulzapps/ttvg.git
```
* Create and cd into a build directory: 
```
mkdir build 
cd build
```
* Initialize conan to install and build the required packages: 
```
conan install ../ttvg -s build_type=Debug --build mising
```
* Initialize CMake: 
```
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

At this point you should makefiles in the folder (Ninja files if that is your generator). You can do a test build by doing
```
cmake --build .
```

### Linux

I have not tried to build this on Linux but there's no reason it shouldn't work on any distribution that support [SFML](http://sfml-dev.org/).

## How to Play

Once the game has been built and you try to run it, then you might notice that it crashes. This is because you must tell the game where to find all of the assets (i.e. images, sound files, etc). You can specify where the resources are with the `--resources` or `-r` option on the command line

```
tooter -r c:\source\ttvg\resources
```

## Liensing

Unless otherwise noted, this code is licensed under the MIT license.

Artwork and audio files are all either public domain, GPL or used under fair use. 

## Links

For more information visit [Dog Finger Studios](https://dogfinger.com).