#!/bin/bash

git branch android-ci
git checkout android-ci
cp ci/ando .travis.yml
git checkout master
git branch emscripten-ci
git checkout emscripten-ci
cp ci_emsc .travis.yml
git checkout master
git push origin master
git push origin android-ci
git push origin emscripten-ci
git branch -d android-ci
git branch -d emscripten-ci
