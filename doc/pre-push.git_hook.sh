#!/bin/sh

remote="$1"
url="$2"

if [ -z ${IS_IN_PRE_PUSH+x} ]; then echo Foo; else exit 0; fi
export IS_IN_PRE_PUSH=true
current=$(git branch | grep \* | cut -c 3-80)
git branch -D -q android-ci
git branch -D -q ios-ci
git branch -D -q emscripten-ci
git branch android-ci
git branch ios-ci
git branch emscripten-ci
git checkout android-ci
cp ci/travis/travis_android.yml .travis.yml
git push origin android-ci
git checkout ios-ci
cp ci/travis/travis_ios.yml .travis.yml
git push origin ios-ci
git checkout emscripten-ci
cp ci/travis/travis_emscripten.yml .travis.yml
git push origin emscripten-ci
git checkout $current
unset IS_IN_PRE_PUSH
