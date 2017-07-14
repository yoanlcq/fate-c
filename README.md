As of July 2017, this is no maintained anymore, because I'm rewriting it in Rust. This repo is now named `fate-c` and the rewrite is under another repo named `fate`.

| **OS X Mavericks and Ubuntu 12.04 64-bits - GCC and Clang**  | [![Travis build Status](https://travis-ci.org/yoanlcq/FATE.png?branch=master)](https://travis-ci.org/yoanlcq/FATE)  |
|---|---|
| **Windows - GCC and MSVC**  | [![Build status](https://ci.appveyor.com/api/projects/status/ca669xiyxll64vec?svg=true)](https://ci.appveyor.com/project/yoanlcq/fate)  |
  
*I'm actually building a game - It just so happens that I know how to separate concerns.*  

# FATE, the Fight And Travel (Game) Engine.  
- **Main focuses** : control, performance, and huge universes. All of this implies basic knowledge of the hardware and good knowledge of the data.  
- **Compilers**: GCC, Clang and MSVC.
- **Targets**: Windows, Linux, OS X, Android, iOS, Emscripten (and probably FreeBSD, but untested).  

The OpenGL version is required to be 3.0 or greater on desktop, and ES 2 or greater on other targets.  
    
You can play an online demo [here](http://yoanlecoq.com/dl/dev/fate/demo/index.html) (I acknowledge the ~15s loading time, I need to figure out what's causing it).  
[See the README](https://github.com/yoanlcq/FATE/tree/master/examples/cube) for asset credits.  
![Rainbow Dice Demo Screenshot](http://yoanlecoq.com/dl/dev/fate/fate0_2_0_rainbow_dice.png)  
  
The code's documentation is either lacking or out-of-date, because the internal APIs evolve fast, and right now getting actual things done is my priority.  
The demo (`examples/cube`) does too many things by itself - most tasks such as rendering with OpenGL are yet to be moved into the engine, which must offer a backend-agnostic API for rendering.  
I'd like the engine to offer a data-oriented sort of Entity-Component-System API which would manage the data flow in a way that solves the actual problems my games will have.  

## Why C instead of C++ ?
Many valid reasons. If you really want to know, [this article](http://crafn.kapsi.fi/new_engine.html) (not from me) just hits the nail on the head.  
While you're at it, [learn about Data-Oriented Design](https://github.com/taylor001/data-oriented-design) and how [OOP sucks](http://harmful.cat-v.org/software/OO_programming/). [Another article ?](https://blog.grumpycoder.net/index.php/post/2012/03/02/Why-C-is-a-good-language-or-why-you-re-wrong-thinking-C-is-better)

## So what do we have here ?
- **Flexible GNU-Make-based build system**, which gives a lot of control over user-defined builds and per-file C compiler flags.   See `examples/cube/fatefile.mk`. See also the `*.cflags` files in, for instance, `src/fate/`.
- The demo at `examples/cube` runs sucessfully on **Linux, Windows, Android and Emscripten**.  
  It would very probably work on OS X and uh... maybe (??) on iOS (need to test next time I boot my Mac Mini).
- Common platform-specific definitions and macros wrapping compiler attributes;
- **Compiler-Assisted Instrumentation (CAI)**, configurable on a per-build basis.
- Platform-agnostic **resource loading and persistent storage**, with the `fe_iov`, `fe_fd` and `fe_fpath` APIs.  
  I'm happy with their cleanliness, especially after all of the redesigns they have gone through.
- Platform-agnostic **synchronous sockets** wrapper (wasn't that hard actually);
- Wrapper around **Linux's `perf_event` API**, which promises profile information about cache usage and a lot more.
- **"Hardware" module**, which provides extensive info about the CPU and wraps some cool compiler intrinsics;
- **OpenGL-side** : wrapped GLAD/GLEW behaviour, debug context utilities, memory info, program builder, pipeline stats, and some other stuff;
- Extensive (and flexible) **vector math library** (generated using GNU M4), based on either naive implementations or compiler-specific vector extensions;
- **Miscellaneous** : logging, stack and heap allocation, struct introspection (optional, requires declaring structs with macros), table of display resolutions, crash handler...
- Doxygen documentation (yet to be updated though).

## What's yet to do ?
- Multi-threading and Task graphs; The engine would have a default task graph in which the user can schedule its game-specific updates;
- Provide functionality through Components and Systems.
- Actually implement the asynchronous part of the `fe_iov` API.
- The WebSocket13 server;
- 3D backend-agnostic rendering, with at least partial Direct3D10 and Vulkan support;
- Audio support through OpenAL;
- Matrix library (not that hard, just a generalization of [this header](http://github.com/datenwolf/linmath));
- Physics and Fixed-point numbers (see [this side-project of mine](http://github.com/yoanlcq/boulette-physiques));
- The i18n (Internationalization) and l10n (Localization) tools and modules;
- Interoperability with the Steamworks SDK;
- Keep that doc up-to-date! Also update the license at the top of each file.

## What are the requirements ?
Short answer :
- **Desktop** : GNU make, a C compiler (one of GCC, Clang and MSVC), and SDL2. 
- **Android** : [See here for now](https://github.com/yoanlcq/FATE/wiki/Android-setup).
- **iOS** : [See here for now](https://github.com/yoanlcq/FATE/wiki/Setting-up-SDL2-for-iOS-on-XCode-7.2).
- **Emscripten** : Just the Emscripten SDK.
  
Feel free to take a look at the CI scripts (`.travis.yml` and `.appveyor.yml`) for details.  
I'm anyway planning to write a script that automates the installation process for various targets (see `bin/*/fe-setup*`).  

## Philosophy, Inspirations
- **No one knows better than you how to solve your particular problems.** *Not even the compiler. Not even the hardware.*  
  FATE, by design, will not assume you're an idiot and make critical decisions on your behalf - instead it'll do its best to provide you with maximum control and help you taking advantage of it.  
  In my opinion, modern engines and languages dramatically fail at it, their excuse being "we're beginner-friendly". I do believe that "beginner" and "full control" are not mutually exclusive.  
  By the way, the engine should make it easy even for the beginner to write efficient data-oriented code. It's just a matter of well-written and illustrated tutorials.
- **Support for the minority**: Not everyone has a crazy gamer PC to run games on, and not everyone is on Windows.  
  As a player, if my PC sucks, I **want** your game to run smoothly even with all graphics settings set to minimum - that's also what I expect from FATE : we're making games, not PIXAR-quality interactive movies.  
  You'd think that today everyone's average machine is already powerful, but not only this isn't true (they're still expensive), but more and more applications just take resources for granted and waste them in exchange for what their developers think is "pretty-looking" code.
- The "F" in "FATE" is for **"Fighting"**. [M.U.G.E.N](https://en.wikipedia.org/wiki/M.U.G.E.N) is a huge inspiration for its simplicity and yet the various possibilities it opens up. A M.U.G.E.N clone might even be implemented using FATE someday.
- The "T" in "FATE" is for **"Travel"**. FATE should not only allow for possibly infinite worlds, but also allow the viewer to move at extreme speeds.   
  Since disk I/O would be the biggest overhead, the engine must make it easy to create worlds that load fast.  
  The world shouldn't be "streaming" because it implies we hang on to chunks that we may be way past from when moving fast: Load resources asynchonously, don't queue them.  
- **Your game is your editor**. It would just so happen that FATE provides a default feature-complete GUI within your game, but you could very well have written it yourself. It'd be far from being a "black box".
- **In-game reloading of code and data**. FATE will use your OS's file monitoring API to wait for changes on the resource files, and reload them when needed.  
  Source files would compile to DLLs. No need to restart the game to see changes in action, and it's still all in C;

