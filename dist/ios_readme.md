# Prerequisites
Download the [SDL 2.0.4 sources](https://www.libsdl.org/release/SDL2-2.0.4.zip) and unzip the archive;  
export SDLPATH="PATH_TO_SDL"  
Open $SDLPATH/Xcode-iOS/SDL/SDL.xcodeproj with Xcode  

# Building libSDL2.a for Simulator
At the top of the screen, right of the ▶️  button, then right of `libSDL`, make sure you select a Simulator device (I chose `iPhone 4s`);  
Click `SDL` on the top-left, then `Build Settings`. Make sure you choose `All` just under (instead of `Basic`), then edit the following fields under the *Architectures* dropdown : 
- `Architectures` and `Valid Architectures` must contain `i386 x86_64 armv7 armv7s arm64`;
- `Build Active Architecture Only` should be set to *No*;  

Your screen should look quite like this :  
![Build Settings](https://dl.dropboxusercontent.com/u/76675545/ios_wiki/ios_wiki_0.jpg)  
Then hit the "Build" button (▶️ ).  

# Building libSDL2.a for actual devices
This is the same as the previous steps, only with these differences : 
- Select `Generic iOS Device` as the target;
- `Architectures` must contain `armv7 armv7s arm64` (that is, WITHOUT `i386 x86_64`);  

Again, here's a screenshot you can refer to.
![Build settings](https://dl.dropboxusercontent.com/u/76675545/ios_wiki/ios_wiki_1.jpg)  
Then hit the "Build" button (▶️ ).  
 
# Building the SDL iOS Application Template
This one is easy : reproduce the following screenshot, then hit build :  
![screenshot](https://dl.dropboxusercontent.com/u/76675545/ios_wiki/ios_wiki_2.png)  
  
# Setting up your project
Let's retrieve our template and copy it for our project : Right-click on `libSDL.a` under `Products` on the left of the screen, then `Show in Finder`.  
![screenshot](https://dl.dropboxusercontent.com/u/76675545/ios_wiki/ios_wiki_3.png)  
![screenshot](https://dl.dropboxusercontent.com/u/76675545/ios_wiki/ios_wiki_4.png)  

The template is located nearby :  
![screenshot](https://dl.dropboxusercontent.com/u/76675545/ios_wiki/ios_wiki_5.png)  

You may want to copy the `SDL iOS Application` folder somewhere else : this is the project we'll attempt to build.  
Then, copy `Debug-iphonesimulator/libSDL2.a` to your `SDL_iOS_Application/SDL/lib`:  
![screenshot](https://dl.dropboxusercontent.com/u/76675545/ios_wiki/ios_wiki_6.png)  

Finally, move all files from `SDL_iOS_Application/SDL/include` to `SDL_iOS_Application/SDL/include/SDL2`, like so :  
![screenshot](https://dl.dropboxusercontent.com/u/76675545/ios_wiki/ios_wiki_8.png)  
  
Then edit the library and headers search paths, like so : 
![screenshot](https://dl.dropboxusercontent.com/u/76675545/ios_wiki/ios_wiki_9.png)  
  
And finally, in your source files, replace `#include "SDL.h"` by the most correct `#include <SDL2/SDL.h>` :  
![screenshot](https://dl.dropboxusercontent.com/u/76675545/ios_wiki/ios_wiki_7.png)  
  
Make sure you're targeting a Simulator device, then hit "Build".
