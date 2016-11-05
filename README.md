[![Travis build Status](https://travis-ci.org/yoanlcq/FATE.png?branch=master)](https://travis-ci.org/yoanlcq/FATE) (OS X Mavericks and Ubuntu 12.04 64-bits - GCC and Clang)    
[![Build status](https://ci.appveyor.com/api/projects/status/ca669xiyxll64vec?svg=true)](https://ci.appveyor.com/project/yoanlcq/fate) (Windows - GCC and MSVC)  

# FATE
Fight And Travel (Game) Engine. A very ambitious project that will take years to reach 1.0.  
Main focuses : simplicity, control, performance, and huge universes. All of this implies basic knowledge of the hardware and good knowledge of the data.  
Compilers: GCC, Clang and MSVC. Targets : Windows, Linux, OS X, Android, iOS, Emscripten (and probably FreeBSD, but untested).
  
You can play an online demo [here](http://yoanlecoq.com/dl/fate_cube_demo) (I acknowledge the ~15s loading time, I need to figure out what's causing it).  
![Rainbow Dice Demo Screenshot](http://yoanlecoq.com/dl/fate_cube_demo/fate0_2_0_rainbow_dice.png)  
  
The code's documentation is either lacking or out-of-date, because the internal APIs evolve fast and at this stage, getting actual things done is my priority.  
The demo (`examples/cube`) does too many things by itself - most tasks such as rendering with OpenGL are yet to be moved into the engine, which must offer a backend-agnostic API for rendering. I'd like the engine to offer a data-oriented sort of Entity-Component-System API which would manage the data flow in a way that solves the actual problems games have.  

## Philosophy, Inspirations
- Support for the minority. Not everyone has a crazy gamer PC to run games on, and not everyone is on Windows. It is part of F.A.T.E's philosophy to make a best effort in providing compatible, efficient and cross-platform games.
- Data-Oriented Design: Traditionally, it's the engine that's supposed to know the hardware and complicated APIs, and since the user "is too dumb for this", it provides easy-to-use abstractions for them. 
  I, for one, think that engines fail to teach beginners how to write code that is efficient for the hardware. It is not that hard, and I do believe that the engine can help the user implement data-oriented design in its own code.
- The "F" in "FATE" is for "Fighting". [M.U.G.E.N](https://en.wikipedia.org/wiki/M.U.G.E.N) is a huge inspiration for its simplicity and yet the various possibilities it opens up. A M.U.G.E.N clone might even be implemented using FATE someday.
- The "T" in "FATE" is for "Travel". FATE should not only allow for possibly infinite worlds, but also allow the viewer to move at extreme speeds. Since disk I/O would be the biggest overhead, the engine must make it easy to create worlds that load fast.
  The world shouldn't be "streaming" because it implies we hang on to chunks that we may be way past from when moving fast: Load resources asynchonously, don't queue them.
- Your game is your editor. It would just so happen that FATE provides a default feature-complete GUI within your game, 
- In-game reloading of code and data. FATE will use your OS's file monitoring API to wait for changes on the resource files, and reload them when needed. The code of each entity compiles to a shared library. No need to restart the game to see changes in action, and it's still all in C;

