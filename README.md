SPACECRAFT
===

## Description

SPACECRAFT is a game prototype which was made by @smealum, @ArthurSonzogni and @posva. Our goal with this was to make a minecraft clone which wouldn't ground the player to a single infinite -- and flat -- world. Instead, we want players to be able to explore an (almost) infinite universe, made of galaxies, solar systems and planets, all the while keeping their ability to place and remove blocks.
Please be aware that this is a prototype which was developed as a school project over the span about three weeks. As such, it is extremely unpolished and incomplete. That being said, we believe it is a solid foundation which could be used to make our vision a reality.

The github repo for this can be found at https://github.com/smealum/SPACECRAFT

## Windows Demo

You can download a precompiled demo for windows at https://dl.dropboxusercontent.com/u/13279485/spacecraft.zip

## Dependencies

All included libraries save for the windows version were compiled for the x86_64 architecture.

If you'd rather install the libraries yourself, you'll need :

- glfw3 (used for window management/opengl context stuff)
- AntTweakBar (for easy testing tools)
- libnoise (for planet generation)
- SFML 2.1 (for threads)

Please note that this code was intended to be written in C++11; however, as our school's computers aren't super up to date, we had to limit ourselves to only a subset of that standard.

## Video

Here's a video showing what the engine looks like as of June 19th 2014 : https://www.youtube.com/watch?v=HsskVpPskSc

## Screenshots

![galaxy](http://i.imgur.com/KczAZt2.png)

![space](http://i.imgur.com/i5h5K6r.png)

![sunset](http://i.imgur.com/uY6xw1H.png)

![blocks](http://i.imgur.com/qqypYwj.png)

## Compiling

By default, compilation is done in release mode. Build mode can still be specified manually however.

Compiling in debug mode :

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=DEBUG ..
```

Compiling in release mode :

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=RELEASE ..
```

- **Windows**:
If the provided pre-compiled libraries do not work as expected, you should compile them yourself and place them in your MinGW floder. You'll then have to add your MinGW path to the CMakeLists.txt file, line 27 and 28 :

```
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} C:/CodeBlocks/MinGW/lib ${PROJECT_SOURCE_DIR}/extlibs/libs-win32)
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} C:/CodeBlocks/MinGW/include )
```

```
cmake -G "MinGW Makefiles" ..
mingw32-make
```

## Running 

The easiest way to run SPACECRAFT is to use the following command :
```
make run
```
Once compiled, you may also run it using :

```
./bin/spacecraft
```

Under Mac OS X, you might have no choice but to use that first option as we do not use a .app directory.

## Controls

As this is an engine prototype rather than an actual game, the controls are very awkward and complicated to use.
There are two modes of control : no-clip flying camera, and player mode. The former can be used in any situation, while the latter should only be used to navigate planets' surfaces.

No-clip flying camera controls :
```
W/S : move forward/backwards
A/D : move left/right
Q/E : move up/down

I/K : control pitch
J/L : control yaw
U/O : control roll

Y/H: control movespeed

SPACE : toggle mouse-look mode
```

Player mode controls :
```
W/S : move forward/backwards
A/D : move left/right

I/K : control pitch
J/L : control yaw

SPACE : jump
T : toggle mouse-look mode

LEFT CLICK / C : place block
RIGHT CLICK / X : remove block
```

Common controls :

```
P/M : control time
SHIFT + P/M : control time faster
```

## Unit tests

In order to run the few unit tests we wrote, you should use the following command :

```
make && make test
```

If you run into an error, try the following :

```
cmake .. # on considère qu'on est dans build/
make
make test
```

On Mac OS X, you should run `make osxfix` before running the unit tests.

As these tests were written with UNIX in mind, there's no guarantee they'll work properly on Windows systems.

## Credits

The vast majority of the code present in SPACECRAFT is original and was written by @smealum, @ArthurSonzogni and @posva. In the same way, the application's design and architecture is completely original.
That being said, some bits and pieces were borrowed or heavily inspired by the work of others. These bits include :

- The atmospheric scattering code, which was originally written by Sean P O'Neil. We essentially took his CPU-based code, made it into a fragment shader and changed a couple things (including adding a new LUT).
- The planet heightmap generation code, which is largely inspired/stolen from a libnoise example.
- The block textures, which were taken from the Eldpack Minecraft texturepack, by eldrone.
- GLM. 
- Probably other stuff I'm forgetting; please let us know if you believe we're not doing your work justice.

## Contact

@smealum : http://smealum.net, sme@lum.sexy
@ArthurSonzogni : http://panigame.fr, ArthurSonzogni@panigame.fr
@posva : http://posva.net, posva13@gmail.com
