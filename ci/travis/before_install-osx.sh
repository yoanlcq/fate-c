#!/bin/bash

brew update;
brew tap homebrew/versions;
brew install gcc49;
brew link --overwrite gcc49;
cd /usr/local/bin
sudo ln -vsf gcc-4.9 gcc
cd ~
wget https://www.libsdl.org/release/SDL2-2.0.4.dmg;
# From http://commandlinemac.blogspot.fr/2008/12/installing-dmg-application-from-command.html
hdiutil mount SDL2-2.0.4.dmg
echo On fait tourner les serviettes
ls /Volumes
sudo cp -R "/Volumes/SDL2-2.0.4/SDL2.framework" /Library/Frameworks
hdiutil unmount "/Volumes/SDL2-2.0.4/"

