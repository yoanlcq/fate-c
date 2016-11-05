Travis build status - OS X Mavericks and Ubuntu 12.04 64-bits - gcc and clang :  
[![Travis build Status](https://travis-ci.org/yoanlcq/FATE.png?branch=master)](https://travis-ci.org/yoanlcq/FATE)
  
AppVeyor build status - Windows 64 - gcc and MSVC :  
[![Build status](https://ci.appveyor.com/api/projects/status/ca669xiyxll64vec?svg=true)](https://ci.appveyor.com/project/yoanlcq/fate)


# FATE
Fight And Travel (Game) Engine. A very ambitious project that will take years to reach 1.0.  
Main focuses : simplicity, control, performance, and huge universes. All of this implies basic knowledge of the hardware and good knowledge of the data.  
Compilers: GCC, Clang and MSVC. Targets : Windows, Linux, OS X, Android, iOS, Emscripten (and probably FreeBSD, but untested).
  
You can play an online demo [here](http://yoanlecoq.com/dl/fate_cube_demo) (I acknowledge the ~15s loading time, I need to figure out what's causing it).  
![Rainbow Dice Demo Screenshot](http://yoanlecoq.com/dl/fate_cube_demo/fate0_2_0_rainbow_dice.png)  
  
The code's documentation is either lacking or out-of-date, because the internal APIs evolve fast and at this stage, getting actual things done is my priority.  
The demo (`examples/cube`) does too many things by itself - most tasks such as rendering with OpenGL are yet to be moved into the engine, which must offer a backend-agnostic API for rendering. I'd like the engine to offer a data-oriented sort of Entity-Component-System API which would manage the data flow in a way that solves the actual problems games have.  

## Inspirations
- [M.U.G.E.N](https://en.wikipedia.org/wiki/M.U.G.E.N) is a very-well designed fighting game engine. Its concepts will help shaping the "F" in "FATE".
- The "T" in "FATE" is for "Travel". FATE should not only allow for possibly infinite worlds, but also allow the viewer to move at extreme speeds.
- Data-Oriented Design principles, all the way from the engine to user code.
- In-game reloading of code and data. FATE will use your OS's file monitoring API to wait for changes on the resource files, and reload them when needed. The code of each entity compiles to a shared library. No need to restart the game to see changes in action, and it's still all in C;
- Support for the minority. Not everyone has a crazy gamer PC to run games on, and not everyone is on Windows. It is part of F.A.T.E's philosophy to make a best effort in providing compatible, efficient and cross-platform games.
