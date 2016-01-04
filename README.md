Travis build status - OS X Mavericks and Ubuntu 12.04 64-bits :  
![Travis build status](https://travis-ci.org/yoanlcq/FATE.svg?branch=master)

# FATE
Fight And Travel (Game) Engine. A very ambitious project that will take years to reach 1.0.
  
A spec for the engine is being written under `doc/`.
  
And here's a cube that represents the current progress for the OpenGL rendering backend.  
![Screenshot](http://yoanlecoq.com/dl/fatewip.png)
  
  
**Please treat this repository as it is : _An IT student's work in progress_.**  
It is **NOT** yet suited for review or contributions.  

## Inspiration and features
This is what makes the difference :
- [M.U.G.E.N](https://en.wikipedia.org/wiki/M.U.G.E.N) is a very-well designed fighting game engine. Its concepts will help shaping the "F" in "FATE".
- The "T" in "FATE" is for "Travel". FATE should not only allow for possibly infinite worlds, but also allow the viewer to move at extreme speeds.
- Virtual Filesystems. The whole engine's core will be a special in-memory Filesystem. This will help organise the huge world, provide Symbolic Links, provide Hard Links, which all also allow for a nodal approach.
- If time allows it, an intermediate language, very similar to M.U.G.E.N's, that translate to C or Javascript;
- In-game reloading of code and data. FATE will use your OS's file monitoring API to wait for changes on the resource files, and reload them when needed. The code of each entity compiles to a shared library. No need to restart the game to see changes in action, and it's still all in C;
- Support for the minority. Not everyone has a crazy gamer PC to run games on, and not everyone is on Windows. It is part of F.A.T.E's philosophy to make a best effort in providing compatible, efficient and cross-platform games.
