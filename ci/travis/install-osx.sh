#!/bin/bash

brew tap homebrew/versions;
brew install gcc49;
cd /usr/local/bin
sudo ln -sf gcc-4.9 gcc
cd ~
# From http://commandlinemac.blogspot.fr/2008/12/installing-dmg-application-from-command.html
hdiutil mount SDL-2.0.4.dmg
sudo cp -R "/Volumes/SDL-2.0.4/SDL2.framework" /Library/Frameworks
hdiutil unmount "/Volumes/SDL-2.0.4/"

