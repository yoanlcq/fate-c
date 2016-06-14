#!/bin/bash

brew update > /dev/null;
brew tap homebrew/versions;
brew install gcc49;
brew link --overwrite gcc49;
pushd /usr/local/bin
sudo ln -vsf gcc-4.9 gcc
popd
mkdir tmp
pushd tmp
wget https://www.libsdl.org/release/SDL2-2.0.4.dmg;
hdiutil mount SDL2-2.0.4.dmg
sudo cp -R /Volumes/SDL2/SDL2.framework /Library/Frameworks
hdiutil unmount /Volumes/SDL2/
popd
rm -rf tmp
brew install gettext
brew link --force gettext
