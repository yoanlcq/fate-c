#!/bin/bash

yes | sudo add-apt-repository ppa:ubuntu-toolchain-r/test;
yes | sudo add-apt-repository ppa:zoogie/sdl2-snapshots;
sudo apt-get update -qq;

