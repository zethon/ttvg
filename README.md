# The Tommy Tooter Video Game

![](docs/images/screenshot1.png)

This is a 2D RPG game written in C++ using [SFML](http://sfml-dev.org/) for multimedia. This game runs on Windows, Mac and Linux.

## How to Install

Currently there is no installer, so the game must be built manually. An installer for both Windows and Mac will be created as the game matures and nears a releaseable state.

## How to Build

All platforms require: 

* [Git](https://git-scm.com/downloads)
* [Git Large File Storage](https://git-lfs.github.com/)
* [CMake](https://cmake.org/download/)
* [conan 1.27+](https://conan.io/)

The two following conan repos must be added:

```
conan remote add SFML https://api.bintray.com/conan/bincrafters/public-conan
conan remote add owl https://api.bintray.com/conan/zethon/owl
```


### Windows

I have been using [Visual Studio 2017](https://visualstudio.microsoft.com/vs/older-downloads/) as my toolchain, even tough I've actually been using Visual 2019 as my editor/IDE. I've yet to figure out how to get conan to play nicely with Visual Studio 2019, but since VS2019 can load and use VS2017 solution files without a problem, this method works fine.

The [OpenAL Libraries](https://openal.org/downloads/) need to be installed for sound files to load on Windows. 

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
cmake .. "-GVisual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Debug
```

If all goes well you should have a **ttvg.sln** in the `build` folder. This is a Visual Studio solution file that can be loaded and you're ready to start coding.

### Mac

Development requires OSX 10.15 or higher. A C++17 compiler is also required.

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
* Initialize CMake. This will create makefiles. If you want to use Xcode change the generator (the `-G` argument). See the Windows instructions for an example of how to specify the generator.

```
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

At this point you should have makefiles in the folder (Ninja files if that is your generator). You can do a test build by doing
```
cmake --build .
```

### Linux

Install Dependencies (Ubuntu 19.10)

```
sudo apt install cmake
sudo apt install libgl1-mesa-dev
sudo apt install libudev-dev
sudo apt install libfreetype6-dev
sudo apt install libxcb-render-util0-dev
sudo apt install libxcb-xkb-dev
sudo apt install libxcb-icccm4-dev
sudo apt install libxcb-image0-dev
sudo apt install libxcb-keysyms1-dev
sudo apt install libxcb-xinerama0-dev
```

Install Dependencies (Ubuntu 20.04)
```
sudo apt install libxcb-shape0-dev
sudo apt install libxcb-xfixes0-dev
```

Run Build

```
tools/linux/build.sh
```

Run Game
```
tools/linux/run.sh
```

## How to Play

Once the game has been built and you try to run it, then you might notice that it crashes. This is because you must tell the game where to find all of the assets (i.e. images, sound files, etc) Eventually installers will install the resource files to appropriate locations where the executable will be able to find them automatically. For now you must manually tell the game where the resources are with the `--resources` or `-r` command line parameter. For example:

```
tooter -r c:\source\ttvg\resources
```

The path specificied should be the `resources` path in this repo on your local system.

## Licensing

Unless otherwise noted, this code is licensed under the MIT license.

Artwork and audio files are all either public domain, GPL or used under fair use. 

## Links

For more information visit [Dog Finger Studios](https://dogfinger.com).
