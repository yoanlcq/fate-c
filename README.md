# FATE
Fight And Travel (Game) Engine. A very ambitious project that will take years to reach 1.0.
  
![Screenshot](yoanlecoq.com/dl/fatewip.png)
  
  
Please treat this repository as it is : A big "IT student's Work In Progress".  
It is NOT yet suited for review or contributions.  
Therefore, I advise you wait a year or two before coming back.
  
However, if you feel like reading bullet lists, here you go.
  
## Yet another game engine...
Indeed, there are countless existing game engines out there. Why reinvent the wheel ? That would be such a waste of time.  
Well, over the past few years, I have indeed tested all kinds of game engines, believing faithfully that there would be one that would fit my criterias.  
But most of them couldn't survive the deadly "viewer moving at 1000 Kmph" test, which is vital for a game I have in mind.

And over time, the reasons to make my own engine kept adding up. Here is a non-exhaustive list :  
- Most game engines ship either with Windows, OS X or Linux, but not the three of them;
- Those that claim to be "Open-World" either still have limits, or can't keep up with the viewer moving at 1000 Kmph because the world resources are queued synchronously;
- Most have never thought to implement [Floating Origin](http://www.floatingorigin.com/);
- I believe that, by rolling my own, I'll understand better the design decisions that led to today's well known engines;
- Some require a restart when changing some settings (like video backends). As of year 2015, I find that ridiculous.
- Some don't run with a fully-compiled language. Sorry, I aim for performance and nothing in my way.
- Don't tell me about UE4, it burns your hardware even with an empty flat world.
  
## Inspiration and features
This is what makes the difference :
- [M.U.G.E.N](https://en.wikipedia.org/wiki/M.U.G.E.N) is a very-well designed fighting game engine. Its concepts will help shaping the "F" in "FATE".
- The "T" in "FATE" is for "Travel". FATE should not only allow for possibly infinite worlds, but also allow the viewer to move at extreme speeds.
- Virtual Filesystems. The whole engine's core will be a special in-memory Filesystem. This will help organise the huge world, provide Symbolic Links, provide Hard Links, which all also allow for a Nodal approach.
- An intermediate language, very similar to M.U.G.E.N's, that translate to C or Javascript;
- In-game reloading of code and data. FATE will use your OS's file monitoring API to wait for changes on the resource files, and reload them when needed. The code of each entity compiles to a shared library. No need to restart the game to see changes in action, and it's still all in C;
- Support for the minority. Not everyone has a crazy gamer PC to run games on. And not everyone is on Windows. I want my games to run on your platform, and well.
